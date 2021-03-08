#include "pch.h"
#include "LightTexRenderer.h"
#include "..\Common\DirectXHelper.h"
#include "DDSTextureLoader.h"


using namespace Cloth;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
LightTexRenderer::LightTexRenderer(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    ObjectShape objectShape,
    FileLoad& fileUtil,
    const std::wstring& filename) :
    ObjectRenderer(deviceResources, objectShape, fileUtil)
{
    m_pObjectVertices = nullptr;
    OBJECT_RENDERER_SET_VERTICES(PlaneTex, planeTex);
    OBJECT_RENDERER_SET_VERTICES(SphereTex, sphereTex);
    OBJECT_RENDERER_SET_VERTICES(TeapotTex, sphereTex);
    OBJECT_RENDERER_SET_VERTICES(TorusTex, torusTex);
    OBJECT_RENDERER_SET_VERTICES(HiResPlaneTex, hiresPlaneTex);
    OBJECT_RENDERER_SET_VERTICES(WallsTex, wallsTex);

    if (nullptr == m_pObjectVertices)
    {
        OUTPUT_LOG_MSG("Invalid Textured Object");
        DX::ThrowIfFailed(-1);
    }

    auto fileData       = m_fileUtil.ReadFileSync(filename);
    const byte* pData   = &fileData[0];
    size_t  dataSize    = fileData.size();
    auto context        = m_deviceResources->GetD3DDeviceContext();
    auto device         = m_deviceResources->GetD3DDevice();

    CreateDDSTextureFromMemory(
        device,
        pData,
        dataSize,
        m_texture.GetAddressOf(),
        m_textureView.GetAddressOf());

    // Create the sampler state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = device->CreateSamplerState(
        &sampDesc,
        m_textureSamplerLinear.GetAddressOf());
    OUTPUT_HRESULT(hr);

    D3D11_RASTERIZER_DESC noCullDesc;
    ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
    noCullDesc.FillMode = D3D11_FILL_SOLID;
    noCullDesc.CullMode = D3D11_CULL_NONE;
    noCullDesc.FrontCounterClockwise = false;
    noCullDesc.DepthClipEnable = true;

    hr = device->CreateRasterizerState(&noCullDesc, m_NoCullRS.GetAddressOf());
    OUTPUT_HRESULT(hr);

    SceneRenderer::CreateVertexNormals(
        m_pObjectIndices,
        m_objectIndexCount,
        m_pObjectVertices,
        m_objectVertexCount);

    // Set default values for directional light
    m_LightData.cbDirectionalLight.dir          = XMFLOAT4(0.f, -1.f, 0.f, 1.f);
    m_LightData.cbDirectionalLight.color        = XMFLOAT3(0.f, 1.f, 0.f);
    m_LightData.cbDirectionalLight.intensity    = 0.75f;
    m_LightData.cbDirectionalLight.specPow      = 8.f;

    // Set default values for point light
    m_LightData.cbPointLight.pos            = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
    m_LightData.cbPointLight.color          = XMFLOAT3(1.f, 1.f, 0.f);
    m_LightData.cbPointLight.intensity      = 0.5f;
    m_LightData.cbPointLight.specPow        = 16.f;
    m_LightData.cbPointLight.attenuation    = XMFLOAT3(0.f, 0.f, 1.f);
    m_LightData.cbPointLight.range          = 3.f;


    // Set default values for spot light
    m_LightData.cbSpotLight.pos             = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
    m_LightData.cbSpotLight.color           = XMFLOAT3(1.f, 1.f, 0.f);
    m_LightData.cbSpotLight.intensity       = 0.5f;
    m_LightData.cbSpotLight.specPow         = 64.f;
    m_LightData.cbSpotLight.attenuation     = XMFLOAT3(0.f, 0.f, 1.f);
    m_LightData.cbSpotLight.range           = 5.f;
    m_LightData.cbSpotLight.dir             = XMFLOAT3(0.f, -1.f, 1.f);
    m_LightData.cbSpotLight.specPow         = 8.f; // approx 45 degrees
}

////////////////////////////////////////////////////////////////////////////////
void LightTexRenderer::Init(
    DirectX::XMFLOAT4X4 model,
    DirectX::XMFLOAT4X4 view,
    DirectX::XMFLOAT4X4 projection)
{
    ObjectRenderer::Init(model, view, projection);
}


////////////////////////////////////////////////////////////////////////////////
// Called once per frame updating the model and view matrices.
void LightTexRenderer::Update(CameraData* pCamera)
{
    ObjectRenderer::Update(pCamera);
}

////////////////////////////////////////////////////////////////////////////////
void LightTexRenderer::UpdateLightCBuff()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Lock the constant buffer so it can be written to.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = context->Map(m_cbLight.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return;
    }

    LightCBuffPS* dataPtr = (LightCBuffPS*)mappedResource.pData;
    memset(dataPtr, 0x00, sizeof(LightCBuffPS));

    // Common
    dataPtr->cbEyePos = XMFLOAT4(m_pCamera->eye.x, m_pCamera->eye.y, m_pCamera->eye.z, 1.f);

    // Directional light
    XMVECTOR    color = XMLoadFloat3(&m_LightData.cbDirectionalLight.color);
    color *= m_LightData.cbDirectionalLight.intensity;
    XMFLOAT3    lightColorFinal;
    XMStoreFloat3(&lightColorFinal, color);
    dataPtr->cbDirectionalLight.color   = lightColorFinal;
    dataPtr->cbDirectionalLight.dir     = XMFLOAT4(
        m_LightData.cbDirectionalLight.dir.x,
        m_LightData.cbDirectionalLight.dir.y,
        m_LightData.cbDirectionalLight.dir.z,
        1.f);
    dataPtr->cbDirectionalLight.specPow = m_LightData.cbDirectionalLight.specPow;

    // Point light
    color = XMLoadFloat3(&m_LightData.cbPointLight.color);
    color *= m_LightData.cbPointLight.intensity;
    XMStoreFloat3(&lightColorFinal, color);
    dataPtr->cbPointLight.pos           = XMFLOAT4(
        m_LightData.cbPointLight.pos.x,
        m_LightData.cbPointLight.pos.y,
        m_LightData.cbPointLight.pos.z,
        1.f);
    dataPtr->cbPointLight.color         = lightColorFinal;
    dataPtr->cbPointLight.specPow       = m_LightData.cbPointLight.specPow;
    dataPtr->cbPointLight.attenuation   = m_LightData.cbPointLight.attenuation;
    dataPtr->cbPointLight.range         = m_LightData.cbPointLight.range;

    // Spot light
    color = XMLoadFloat3(&m_LightData.cbSpotLight.color);
    color *= m_LightData.cbSpotLight.intensity;
    XMStoreFloat3(&lightColorFinal, color);
    dataPtr->cbSpotLight.pos = XMFLOAT4(
        m_LightData.cbSpotLight.pos.x,
        m_LightData.cbSpotLight.pos.y,
        m_LightData.cbSpotLight.pos.z,
        1.f);
    dataPtr->cbSpotLight.color          = lightColorFinal;
    dataPtr->cbSpotLight.specPow        = m_LightData.cbSpotLight.specPow;
    dataPtr->cbSpotLight.attenuation    = m_LightData.cbSpotLight.attenuation;
    dataPtr->cbSpotLight.range          = m_LightData.cbSpotLight.range;
    dataPtr->cbSpotLight.dir            = m_LightData.cbSpotLight.dir;
    dataPtr->cbSpotLight.conePow        = m_LightData.cbSpotLight.conePow;

    // Unlock the constant buffer.
    context->Unmap(m_cbLight.Get(), 0);

    // Set the position of the constant buffer in the vertex shader.
    auto bufferNumber = 4;
    // matches : register(b4)

    // Now set the constant buffer in the vertex shader with the updated values.
    context->PSSetConstantBuffers(bufferNumber, 1, m_cbLight.GetAddressOf());
}

////////////////////////////////////////////////////////////////////////////////
void LightTexRenderer::UpdateMaterialCBuff()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    D3D11_MAPPED_SUBRESOURCE mappedResource2;
    auto result = context->Map(m_cbMaterial.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
    if (FAILED(result))
    {
        return;
    }

    // Get a pointer to the data in the constant buffer.
    MaterialCBuffPS*  dataPtr2 = (MaterialCBuffPS*)mappedResource2.pData;
    memset(dataPtr2, 0x00, sizeof(MaterialCBuffPS));

    // material color
    #define MATCOLOR (0.5f)

    dataPtr2->ambient = XMFLOAT4(MATCOLOR, MATCOLOR, MATCOLOR, 1.f);
    dataPtr2->diffuse = XMFLOAT4(MATCOLOR, MATCOLOR, MATCOLOR, 1.f);
    dataPtr2->specular = XMFLOAT4(MATCOLOR, MATCOLOR, MATCOLOR, 1.f);

    // Unlock the constant buffer.
    context->Unmap(m_cbMaterial.Get(), 0);

    // Set the position of the constant buffer in the vertex shader.
    auto bufferNumber2 = 3;
    // matches : register(b3)

    // Now set the constant buffer in the vertex shader with the updated values.
    context->PSSetConstantBuffers(bufferNumber2, 1, m_cbMaterial.GetAddressOf());
}

////////////////////////////////////////////////////////////////////////////////
void LightTexRenderer::Render()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Prepare the constant buffer to send it to the graphics device.
    context->UpdateSubresource(
        m_cbTransform.Get(),
        0,
        NULL,
        &m_cbTransformData,
        0,
        0
        );

    // Update light constant buffers
    UpdateLightCBuff();

    /////////////////////////////////////////////////////////////////////////////////a
    UINT stride = sizeof(GLibAssets::VertexPositionColorNormalTexture);
    UINT offset = 0;
    context->IASetVertexBuffers(
        0,
        1,
        m_vertexBuffer.GetAddressOf(),
        &stride,
        &offset
        );

    context->IASetIndexBuffer(
        m_indexBuffer.Get(),
        DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
        0
        );

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->IASetInputLayout(m_inputLayout.Get());

    // Attach our vertex shader.
    context->VSSetShader(
        m_vertexShader.Get(),
        nullptr,
        0
        );

    // Send the constant buffer to the graphics device.
    context->VSSetConstantBuffers(
        0,
        1,
        m_cbTransform.GetAddressOf()
        );

    // Attach our pixel shader.
    context->PSSetShader(
        m_pixelShader.Get(),
        nullptr,
        0
        );

    ID3D11ShaderResourceView*   pSRV        = m_textureView.Get();
    context->PSSetShaderResources(8, 1, &pSRV);

    ID3D11SamplerState*         pSampler    = m_textureSamplerLinear.Get();
    context->PSSetSamplers(9, 1, &pSampler);

    context->RSSetState(m_NoCullRS.Get());


    // Draw the objects.
    context->DrawIndexed(
        m_objectIndexCount,
        0,
        0
        );

    //context->RSSetState(0);
}

////////////////////////////////////////////////////////////////////////////////
void LightTexRenderer::Create()
{
    // Load shaders asynchronously.
    auto fileData = m_fileUtil.ReadFileSync(L"LightTexVS.cso");

    // After the vertex shader file is loaded, create the shader and input layout.
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateVertexShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &m_vertexShader
            )
        );

    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateInputLayout(
            vertexDesc,
            ARRAYSIZE(vertexDesc),
            &fileData[0],
            fileData.size(),
            &m_inputLayout
            )
        );

    fileData = m_fileUtil.ReadFileSync(L"LightTexPS.cso");

    // After the pixel shader file is loaded, create the shader and constant buffer.
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreatePixelShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &m_pixelShader
            )
        );

    CD3D11_BUFFER_DESC cbDescVertex(sizeof(ModelViewProjDataCBuffVS), D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &cbDescVertex,
            nullptr,
            &m_cbTransform
            )
        );


    // Create light buffers
    CD3D11_BUFFER_DESC cbDescLight;
    cbDescLight.Usage = D3D11_USAGE_DYNAMIC;
    cbDescLight.ByteWidth = sizeof(LightCBuffPS);
    cbDescLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDescLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDescLight.MiscFlags = 0;
    cbDescLight.StructureByteStride = 0;
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &cbDescLight,
            nullptr,
            &m_cbLight
            )
        );

    // Create material buffer
    CD3D11_BUFFER_DESC cbDescMaterial;
    cbDescMaterial.Usage = D3D11_USAGE_DYNAMIC;
    cbDescMaterial.ByteWidth = sizeof(MaterialCBuffPS);
    cbDescMaterial.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDescMaterial.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDescMaterial.MiscFlags = 0;
    cbDescMaterial.StructureByteStride = 0;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &cbDescMaterial,
            nullptr,
            &m_cbMaterial
            )
        );

    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    vertexBufferData.pSysMem = m_pObjectVertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC vertexBufferDesc(m_objectVertexBufferSize, D3D11_BIND_VERTEX_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            &m_vertexBuffer
            )
        );

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = m_pObjectIndices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short)*m_objectIndexCount, D3D11_BIND_INDEX_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            &m_indexBuffer
            )
        );

    /////////////////////////////////////////////////////////////////////////////////
    // Update material constant buffer
    UpdateMaterialCBuff();
}

////////////////////////////////////////////////////////////////////////////////
void LightTexRenderer::Destroy()
{
    ObjectRenderer::Destroy();

    m_cbTransform.Reset();
    m_cbLight.Reset();
    m_cbMaterial.Reset();
}