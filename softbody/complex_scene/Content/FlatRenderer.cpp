#include "pch.h"
#include "FlatRenderer.h"
#include "..\Common\DirectXHelper.h"

using namespace Cloth;
using namespace DirectX;

FlatRenderer::FlatRenderer(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    ObjectShape objectShape,
    FileLoad& fileUtil) :
    ObjectRenderer(deviceResources, objectShape, fileUtil)
{
    OBJECT_RENDERER_SET_ALL_VERTICES();

    SceneRenderer::CreateVertexNormals(
        m_pObjectIndices,
        m_objectIndexCount,
        m_pObjectVertices,
        m_objectVertexCount);
}

void FlatRenderer::Init(
    DirectX::XMFLOAT4X4 model,
    DirectX::XMFLOAT4X4 view,
    DirectX::XMFLOAT4X4 projection)
{
    ObjectRenderer::Init(model, view, projection);
}

// Called once per frame updating the model and view matrices.
void FlatRenderer::Update(CameraData* pCamera)
{
    ObjectRenderer::Update(pCamera);
}

void FlatRenderer::Render()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    // better map
    // http://www.gamedev.net/topic/622685-constant-buffer-updatesubresource-vs-map/
    // Prepare the constant buffer to send it to the graphics device.
    context->UpdateSubresource(
        m_cbTransform.Get(),
        0,
        NULL,
        &m_cbTransformData,
        0,
        0
        );

    // Each vertex is one instance of the VertexPositionColorNormal struct.
    UINT stride = sizeof(GLibAssets::VertexPositionColorNormal);
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

    // Draw the objects.
    context->DrawIndexed(
        m_objectIndexCount,
        0,
        0
        );
}

void FlatRenderer::Create()
{
    // Load shaders asynchronously.
    auto fileData = m_fileUtil.ReadFileSync(L"ColorVS.cso");

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

    // After the pixel shader file is loaded, create the shader and constant buffer.
    fileData = m_fileUtil.ReadFileSync(L"ColorPS.cso");

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreatePixelShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &m_pixelShader
            )
        );

    CD3D11_BUFFER_DESC cbDesc(sizeof(ModelViewProjDataCBuffVS), D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &cbDesc,
            nullptr,
            &m_cbTransform
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
}

void FlatRenderer::Destroy()
{
    ObjectRenderer::Destroy();
}
