#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace soft_body;

using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::System;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_loadingComplete(false),
    m_degreesPerSecond(45),
    m_clothIndexCount(0),
    m_tracking(false),
    m_deviceResources(deviceResources)
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
    m_X = m_Y = m_Z = 0.0f;
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
    Size outputSize = m_deviceResources->GetOutputSize();
    float aspectRatio = outputSize.Width / outputSize.Height;
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // Note that the OrientationTransform3D matrix is post-multiplied here
    // in order to correctly orient the scene to match the display orientation.
    // This post-multiplication step is required for any draw calls that are
    // made to the swap chain render target. For draw calls to other targets,
    // this transform should not be applied.

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );

    XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

    XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

    XMStoreFloat4x4(
        &m_constantBufferDataCloth.projection,
        XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
    );

    // Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
    static const XMVECTORF32 eye = { 0.0f, 3.0f, 8.5f, 0.0f };
    static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
    static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

    XMStoreFloat4x4(&m_constantBufferDataCloth.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));

    // Copy constants
    m_constantBufferDataSphere = m_constantBufferDataCloth;
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
    if (!m_tracking)
    {
        // Convert degrees to radians, then convert seconds to rotation angle
        float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
        double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
        float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

        UpdateCloth(timer);
        radians = 0.f;
        RotateCloth(radians);
        TranslateSphere();
    }
}

void Sample3DSceneRenderer::UpdateCloth(DX::StepTimer const& timer)
{
    DirectX::XMFLOAT3* pEgde = m_particleSystem.GetEdge();

    static float animationCounter = 0.1f;

    for (uint32_t ii = 0; ii < PARTICLES_DIM; ii++)
    {
        pEgde[ii].z = 4.f * sinf(animationCounter);
    }

    animationCounter += 0.01f;

    m_particleSystem.UpdateSphereCenter(m_X, m_Y, m_Z);
    m_particleSystem.Update(timer);

    // Copy particle positions to vertex positions
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    // Disable GPU access to the vertex buffer data.
    auto m_d3dContext = m_deviceResources->GetD3DDeviceContext();
    m_d3dContext->Map(m_vertexBufferCloth.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    // Update the vertex buffer here.
    uint32_t ii = 0;
    for (uint32_t zz = 0; zz < PARTICLES_DIM; zz++)
    {
        for (uint32_t xx = 0; xx < PARTICLES_DIM; xx++)
        {
            m_pClothVertices[ii].pos = m_particleSystem.GetParticlePos(ii);
            ii++;
        }
    }

    // double memory copy
    // TODO: re-work this later
    memcpy(mappedResource.pData, m_pClothVertices, m_clothVerticesSize);

    // Reenable GPU access to the vertex buffer data.
    m_d3dContext->Unmap(m_vertexBufferCloth.Get(), 0);
}

// RotateCloth the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::RotateCloth(float radians)
{
    // Prepare to pass the updated model matrix to the shader
    XMStoreFloat4x4(&m_constantBufferDataCloth.model, XMMatrixTranspose(XMMatrixRotationY(radians)));

}

// Translate sphere
void Sample3DSceneRenderer::TranslateSphere()
{
    XMStoreFloat4x4(&m_constantBufferDataSphere.model,
        XMMatrixTranspose(XMMatrixTranslation(m_X, m_Y, m_Z)));
}

void Sample3DSceneRenderer::StartTracking()
{
    m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
    if (m_tracking)
    {
        float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
        RotateCloth(radians);
    }
}

void Sample3DSceneRenderer::StopTracking()
{
    m_tracking = false;
}

void Sample3DSceneRenderer::RenderCloth()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Prepare the constant buffer to send it to the graphics device.
    context->UpdateSubresource(
        m_constantBufferCloth.Get(),
        0,
        NULL,
        &m_constantBufferDataCloth,
        0,
        0
    );

    // Each vertex is one instance of the VertexPositionColor struct.
    UINT stride = sizeof(VertexPositionColor);
    UINT offset = 0;
    context->IASetVertexBuffers(
        0,
        1,
        m_vertexBufferCloth.GetAddressOf(),
        &stride,
        &offset
    );

    context->IASetIndexBuffer(
        m_indexBufferCloth.Get(),
        DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
        0
    );

    //context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    context->IASetInputLayout(m_inputLayoutCloth.Get());

    // Attach our vertex shader.
    context->VSSetShader(
        m_vertexShaderCloth.Get(),
        nullptr,
        0
    );

    // Send the constant buffer to the graphics device.
    context->VSSetConstantBuffers(
        0,
        1,
        m_constantBufferCloth.GetAddressOf()
    );

    // Attach our pixel shader.
    context->PSSetShader(
        m_pixelShaderCloth.Get(),
        nullptr,
        0
    );

    // Draw the objects.
    context->DrawIndexed(
        m_clothIndexCount,
        0,
        0
    );
}

void Sample3DSceneRenderer::RenderSphere()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Prepare the constant buffer to send it to the graphics device.
    context->UpdateSubresource(
        m_constantBufferSphere.Get(),
        0,
        NULL,
        &m_constantBufferDataSphere,
        0,
        0
    );

    // Each vertex is one instance of the VertexPositionColor struct.
    UINT stride = sizeof(VertexPositionColor);
    UINT offset = 0;
    context->IASetVertexBuffers(
        0,
        1,
        m_vertexBufferSphere.GetAddressOf(),
        &stride,
        &offset
    );

    context->IASetIndexBuffer(
        m_indexBufferSphere.Get(),
        DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
        0
    );

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->IASetInputLayout(m_inputLayoutSphere.Get());

    // Attach our vertex shader.
    context->VSSetShader(
        m_vertexShaderSphere.Get(),
        nullptr,
        0
    );

    // Send the constant buffer to the graphics device.
    context->VSSetConstantBuffers(
        0,
        1,
        m_constantBufferSphere.GetAddressOf()
    );

    // Attach our pixel shader.
    context->PSSetShader(
        m_pixelShaderSphere.Get(),
        nullptr,
        0
    );

    // Draw the objects.
    context->DrawIndexed(
        m_sphereIndexCount,
        0,
        0
    );
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
    RenderCloth();
    RenderSphere();
}

std::vector<byte> Sample3DSceneRenderer::ReadFileSync(const std::wstring& filename)
{
    m_loadingComplete = true;

    auto loadTask = DX::ReadDataAsync(filename);

    auto createTask = loadTask.then([this](const std::vector<byte>& fileData) {
        m_data = fileData;
        }
    );

    createTask.then([this]() {
        m_loadingComplete = false; }
    );

    while (m_loadingComplete)
    {
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }

    std::wstring str = L"loaded = ";
    str += filename;
    str += L"\n";

    OutputDebugStringW(str.c_str());

    return m_data;
}

void Sample3DSceneRenderer::CreateCloth()
{
    auto fileData = ReadFileSync(L"SampleVertexShader.cso");

    // After the vertex shader file is loaded, create the shader and input layout.
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateVertexShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &m_vertexShaderCloth
        )
    );

    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateInputLayout(
            vertexDesc,
            ARRAYSIZE(vertexDesc),
            &fileData[0],
            fileData.size(),
            &m_inputLayoutCloth
        )
    );

    // pixel shader loading
    fileData = ReadFileSync(L"SamplePixelShader.cso");
    // After the pixel shader file is loaded, create the shader and constant buffer.
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreatePixelShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &m_pixelShaderCloth
        )
    );

    CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &constantBufferDesc,
            nullptr,
            &m_constantBufferCloth
        )
    );

    // Once both shaders are loaded, create the mesh.
    // Load mesh vertices. Each vertex has a position and a color.
    m_particleSystem.Init();

    // Load mesh vertices. Each vertex has a position and a color.
    m_pClothVertices = new VertexPositionColor[PARTICLES_DIM * PARTICLES_DIM];
    m_clothVerticesSize = sizeof(VertexPositionColor) * (PARTICLES_DIM * PARTICLES_DIM);

    uint32_t ii = 0;
    for (uint32_t zz = 0; zz < PARTICLES_DIM; zz++)
    {
        for (uint32_t xx = 0; xx < PARTICLES_DIM; xx++)
        {
            m_pClothVertices[ii].pos = m_particleSystem.GetParticlePos(ii);
            m_pClothVertices[ii].color = XMFLOAT3(1.0f, 1.0f, 1.0f);
            ii++;
        }
    }

    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    vertexBufferData.pSysMem = m_pClothVertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC vertexBufferDesc((UINT)m_clothVerticesSize, D3D11_BIND_VERTEX_BUFFER);

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            &m_vertexBufferCloth
        )
    );

    m_clothIndexCount = (PARTICLES_DIM - 1) * 2 * (PARTICLES_DIM) * 2;

    unsigned short* pClothIndices = new unsigned short[m_clothIndexCount];
    UINT clothIndicesSize = m_clothIndexCount * sizeof(unsigned short);

    // horizontal lines
    ii = 0;
    for (uint32_t zz = 0; zz < PARTICLES_DIM; zz++)
    {
        for (uint32_t xx = 0; xx < PARTICLES_DIM - 1; xx++)
        {
            pClothIndices[ii++] = xx + zz * PARTICLES_DIM;
            pClothIndices[ii++] = (xx + 1) + zz * PARTICLES_DIM;
        }
    }

    // vertical lines
    for (uint32_t xx = 0; xx < PARTICLES_DIM; xx++)
    {
        for (uint32_t zz = 0; zz < PARTICLES_DIM - 1; zz++)
        {
            pClothIndices[ii++] = xx + zz * PARTICLES_DIM;
            pClothIndices[ii++] = xx + (zz + 1) * PARTICLES_DIM;
        }
    }

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = pClothIndices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC indexBufferDesc(clothIndicesSize, D3D11_BIND_INDEX_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            &m_indexBufferCloth
        )
    );

    delete[] pClothIndices;
}

void Sample3DSceneRenderer::CreateSphere()
{
    auto fileData = ReadFileSync(L"SampleVertexShader.cso");

    // After the vertex shader file is loaded, create the shader and input layout.
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateVertexShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &m_vertexShaderSphere
        )
    );

    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateInputLayout(
            vertexDesc,
            ARRAYSIZE(vertexDesc),
            &fileData[0],
            fileData.size(),
            &m_inputLayoutSphere
        )
    );

    // pixel shader loading
    fileData = ReadFileSync(L"SamplePixelShader.cso");
    // After the pixel shader file is loaded, create the shader and constant buffer.
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreatePixelShader(
            &fileData[0],
            fileData.size(),
            nullptr,
            &m_pixelShaderSphere
        )
    );

    CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &constantBufferDesc,
            nullptr,
            &m_constantBufferSphere
        )
    );

    // Once both shaders are loaded, create the mesh.
    // Load mesh vertices. Each vertex has a position and a color.
    static VertexPositionColor sphereVertices[] =
    {
        #include "unitsphereVertices.inl"
    };

    m_pSphereVertices = sphereVertices;
    m_sphereVerticesSize = sizeof(sphereVertices);

    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    vertexBufferData.pSysMem = m_pSphereVertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC vertexBufferDesc((UINT)m_sphereVerticesSize, D3D11_BIND_VERTEX_BUFFER);

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            &m_vertexBufferSphere
        )
    );


    // Load mesh indices. Each trio of indices represents
    static unsigned short sphereIndices[] =
    {
        #include "unitsphereIndices.inl"
    };
    m_sphereIndexCount = ARRAYSIZE(sphereIndices);
    UINT sphereIndicesSize = sizeof(sphereIndices);

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = sphereIndices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC indexBufferDesc(sphereIndicesSize, D3D11_BIND_INDEX_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            &m_indexBufferSphere
        )
    );
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
    CreateCloth();
    CreateSphere();
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
    m_loadingComplete = false;
    m_vertexShaderCloth.Reset();
    m_inputLayoutCloth.Reset();
    m_pixelShaderCloth.Reset();
    m_constantBufferCloth.Reset();
    m_vertexBufferCloth.Reset();
    m_indexBufferCloth.Reset();

    delete[] m_pClothVertices;
}

// Handle keybord input
void Sample3DSceneRenderer::OnKeyDown(Windows::System::VirtualKey key)
{
    float inc = 0.2f;
    if (key == VirtualKey::Left)
    {
        OutputDebugStringA("Left Pressed\n");
        m_X -= inc;
    }
    if (key == VirtualKey::Right)
    {
        OutputDebugStringA("Right Pressed\n");
        m_X += inc;
    }
    if (key == VirtualKey::Up)
    {
        OutputDebugStringA("Up Pressed\n");
        m_Z -= inc;
    }
    if (key == VirtualKey::Down)
    {
        OutputDebugStringA("Down Pressed\n");
        m_Z += inc;
    }
}
