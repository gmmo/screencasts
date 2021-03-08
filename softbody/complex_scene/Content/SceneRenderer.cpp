#include "pch.h"
#include "SceneRenderer.h"
#include "GridRenderer.h"
#include "FlatRenderer.h"
#include "LightRenderer.h"
#include "LightTexRenderer.h"
#include "SoftBodyRenderer.h"
#include "..\Common\DirectXHelper.h"

using namespace Cloth;
using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the tetra geometry.
SceneRenderer::SceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_degreesPerSecond(45),
    m_deviceResources(deviceResources),
    m_inputMode(InputMode::InputModeCamera)
{
    CreateWindowSizeDependentResources();
    CreateDeviceDependentResources();
}

////////////////////////////////////////////////////////////////////////////////
void SceneRenderer::CreateVertexNormals(
    unsigned short* pIndexArr,
    uint32 indexArrCount,
    GLibAssets::VertexPositionColorNormal* pVertexArr,
    uint32_t verticesCount)
{
    // Create Vertex Normals
    for (uint32_t ii = 0; ii < indexArrCount; ii += 3)
    {
        unsigned short i0 = pIndexArr[ii];
        unsigned short i1 = pIndexArr[ii + 1];
        unsigned short i2 = pIndexArr[ii + 2];

        XMVECTOR e0 = XMLoadFloat3(&pVertexArr[i1].pos) - XMLoadFloat3(&pVertexArr[i0].pos);
        XMVECTOR e1 = XMLoadFloat3(&pVertexArr[i2].pos) - XMLoadFloat3(&pVertexArr[i0].pos);
        XMVECTOR faceNormal = XMVector3Cross(e0, e1);

        // Add this normal to each vertex
        XMVECTOR    v0Normal = XMLoadFloat3(&pVertexArr[i0].normal) + faceNormal;
        XMVECTOR    v1Normal = XMLoadFloat3(&pVertexArr[i1].normal) + faceNormal;
        XMVECTOR    v2Normal = XMLoadFloat3(&pVertexArr[i2].normal) + faceNormal;

        XMStoreFloat3((XMFLOAT3*)&pVertexArr[i0].normal, v0Normal);
        XMStoreFloat3((XMFLOAT3*)&pVertexArr[i1].normal, v1Normal);
        XMStoreFloat3((XMFLOAT3*)&pVertexArr[i2].normal, v2Normal);
    }

    // Normalize all vertex normals
    for (uint32_t ii = 0; ii < verticesCount; ii++)
    {
        XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&pVertexArr[ii].normal));
        XMStoreFloat3((XMFLOAT3*)&pVertexArr[ii].normal, normal);
    }
}

// TODO: Refactored this
////////////////////////////////////////////////////////////////////////////////
void SceneRenderer::CreateVertexNormals(
    unsigned short* pIndexArr,
    uint32 indexArrCount,
    GLibAssets::VertexPositionColorNormalTexture* pVertexArr,
    uint32_t verticesCount)
{
    // Create Vertex Normals
    for (uint32_t ii = 0; ii < indexArrCount; ii += 3)
    {
        unsigned short i0 = pIndexArr[ii];
        unsigned short i1 = pIndexArr[ii + 1];
        unsigned short i2 = pIndexArr[ii + 2];

        XMVECTOR e0 = XMLoadFloat3(&pVertexArr[i1].pos) - XMLoadFloat3(&pVertexArr[i0].pos);
        XMVECTOR e1 = XMLoadFloat3(&pVertexArr[i2].pos) - XMLoadFloat3(&pVertexArr[i0].pos);
        XMVECTOR faceNormal = XMVector3Cross(e0, e1);

        // Add this normal to each vertex
        XMVECTOR    v0Normal = XMLoadFloat3(&pVertexArr[i0].normal) + faceNormal;
        XMVECTOR    v1Normal = XMLoadFloat3(&pVertexArr[i1].normal) + faceNormal;
        XMVECTOR    v2Normal = XMLoadFloat3(&pVertexArr[i2].normal) + faceNormal;

        XMStoreFloat3((XMFLOAT3*)&pVertexArr[i0].normal, v0Normal);
        XMStoreFloat3((XMFLOAT3*)&pVertexArr[i1].normal, v1Normal);
        XMStoreFloat3((XMFLOAT3*)&pVertexArr[i2].normal, v2Normal);
    }

    // Normalize all vertex normals
    for (uint32_t ii = 0; ii < verticesCount; ii++)
    {
        XMVECTOR    normal = XMVector3Normalize(XMLoadFloat3(&pVertexArr[ii].normal));
        XMStoreFloat3((XMFLOAT3*)&pVertexArr[ii].normal, normal);
    }
}

// Initializes view parameters when the window size changes.
void SceneRenderer::CreateWindowSizeDependentResources()
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

    m_camera.eye = { 0.0f, 2.69f, 3.9f, 0.0f };
    m_camera.at = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_camera.up = { 0.0f, 1.0f, 0.0f, 0.0f };

    DirectX::XMFLOAT4X4 projection;
    XMStoreFloat4x4(&projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

    DirectX::XMFLOAT4X4 view;
    XMStoreFloat4x4(
        &view, XMMatrixTranspose(
            XMMatrixLookAtRH(
                XMLoadFloat4(&m_camera.eye),
                XMLoadFloat4(&m_camera.at),
                XMLoadFloat4(&m_camera.up))));

    DirectX::XMFLOAT4X4 model;
    XMStoreFloat4x4(&model, XMMatrixIdentity());

    m_pCloth= new SoftBodyRenderer(m_deviceResources, ObjectShape::HiResPlaneTex, m_fileUtil, L"Assets\\kids.dds", XMFLOAT3(0.0f, 2.2f, 0.0f));
    m_pCloth->Init(model, view, projection);

    m_pSoftBody = new SoftBodyRenderer(m_deviceResources, ObjectShape::SphereTex, m_fileUtil, L"Assets\\marble.dds", XMFLOAT3(0.0f, 1.2f, 0.5f));
    m_pSoftBody->Init(model, view, projection);

    //m_pSphere = new LightRenderer(m_deviceResources, ObjectShape::Sphere, m_fileUtil);
    //m_pSphere->Init(model, view, projection);

    m_pRoom[0] = new LightTexRenderer(m_deviceResources, ObjectShape::WallsTex, m_fileUtil, L"Assets\\tiles.dds");
    m_pRoom[0]->Init(model, view, projection);

    m_pDirectionalLightSrc = new LightRenderer(m_deviceResources, ObjectShape::Sphere, m_fileUtil);
    m_pDirectionalLightSrc->Init(model, view, projection);

    m_pPointLightSrc = new LightRenderer(m_deviceResources, ObjectShape::Sphere, m_fileUtil);
    m_pPointLightSrc->Init(model, view, projection);

    m_pSpotLightSrc = new LightRenderer(m_deviceResources, ObjectShape::Cone, m_fileUtil);
    m_pSpotLightSrc->Init(model, view, projection);

    m_pGrid = new GridRenderer(m_deviceResources, ObjectShape::Grid, m_fileUtil);
    m_pGrid->Init(model, view, projection);
}

// Called once per frame, rotates the tetra and calculates the model and view matrices.
void SceneRenderer::Update(DX::StepTimer const& timer)
{
    // animate point light
    {
        static float xAnim = 0.02f;
        static float zAnim = -0.01f;
        static float limit = 2.8f;
        XMFLOAT3 pos = m_pPointLightSrc->GetPos();
        pos.x += xAnim;
        if ((pos.x > limit) || (pos.x < -limit)) { xAnim = -xAnim; }
        pos.z += zAnim;
        if ((pos.z > limit) || (pos.z < -limit)) { zAnim = -zAnim; }
        m_pPointLightSrc->SetPos(pos);
    }

    // animate spot light
    {
        static float xAnim = -0.015f;
        static float zAnim = +0.03f;
        static float limit = 2.0f;
        XMFLOAT3 pos = m_pSpotLightSrc->GetPos();
        pos.x += xAnim;
        if ((pos.x > limit) || (pos.x < -limit)) { xAnim = -xAnim; }
        pos.z += zAnim;
        if ((pos.z > limit) || (pos.z < -limit)) { zAnim = -zAnim; }
        m_pSpotLightSrc->SetPos(pos);
    }

    // animate dir light
    {
        static float xAnim = -0.01f;
        static float limit = 2.0f;
        XMFLOAT3 pos = m_pDirectionalLightSrc->GetPos();
        pos.x += xAnim;
        if ((pos.x > limit) || (pos.x < -limit)) { xAnim = -xAnim; }
        m_pDirectionalLightSrc->SetPos(pos);
    }


    UpdateObjectLights();

    m_pDirectionalLightSrc->Update(&m_camera);
    m_pPointLightSrc->Update(&m_camera);
    m_pSpotLightSrc->Update(&m_camera);
    m_pGrid->Update(&m_camera);

    //XMFLOAT3 rot = m_pSphere->GetRot();
    //rot.y += XMConvertToRadians(1.f);
    //m_pSphere->SetRot(rot);
    //m_pSphere->Update(&m_camera);

    m_pRoom[0]->Update(&m_camera);

    m_pSoftBody->Update(timer);
    m_pSoftBody->Update(&m_camera);

    m_pCloth->SetCollidingSphere(m_pSoftBody->GetCenterOfMass(), 0.5f);

    m_pCloth->Update(timer);
    m_pCloth->Update(&m_camera);

}

// Renders one frame using the vertex and pixel shaders.
void SceneRenderer::Render()
{
    m_pRoom[0]->Render();
    //m_pSphere->Render();
    m_pCloth->Render();
    m_pSoftBody->Render();

    m_pDirectionalLightSrc->Render();
    m_pPointLightSrc->Render();
    m_pSpotLightSrc->Render();

    //m_pGrid->Render();
}

void SceneRenderer::CreateDeviceDependentResources()
{
    XMFLOAT3    dirLightPosition    = XMFLOAT3(-1.f, 3.69f, 2.0f);
    XMFLOAT3    pointLightPosition  = XMFLOAT3(-1.4f, 2.5f, -1.3f);
    XMFLOAT3    spotLightPosition   = XMFLOAT3(1.7f, 2.5f, 0.4f);

    float lightSize = 0.05f;
    m_pDirectionalLightSrc->Create();
    m_pDirectionalLightSrc->SetPos(dirLightPosition);
    m_pDirectionalLightSrc->SetScale(XMFLOAT3(lightSize, lightSize, lightSize));

    m_pPointLightSrc->Create();
    m_pPointLightSrc->SetPos(pointLightPosition);
    m_pPointLightSrc->SetScale(XMFLOAT3(lightSize, lightSize, lightSize));

    m_pSpotLightSrc->Create();
    m_pSpotLightSrc->SetPos(spotLightPosition);
    m_pSpotLightSrc->SetScale(XMFLOAT3(lightSize, lightSize, lightSize));

    // Compute light direction
    XMVECTOR    lightDir = XMVector3Normalize(XMLoadFloat3(&m_pDirectionalLightSrc->GetPos()));
    XMFLOAT3    lightDir3;
    XMStoreFloat3(&lightDir3, lightDir);

    LightCBuffPS lightData;
    lightData.cbDirectionalLight.color      = XMFLOAT3(0.75f, 0.75f, 0.75f);
    lightData.cbDirectionalLight.dir        = XMFLOAT4(lightDir3.x, lightDir3.y, lightDir3.z, 1.f);
    lightData.cbDirectionalLight.intensity  = 1.0f;
    lightData.cbDirectionalLight.specPow    = 8.f;

    lightData.cbPointLight.pos              = XMFLOAT4(m_pPointLightSrc->GetPos().x, m_pPointLightSrc->GetPos().y, m_pPointLightSrc->GetPos().z, 1.f);
    lightData.cbPointLight.color            = XMFLOAT3(1.f, 0.5f, 0.f);
    lightData.cbPointLight.intensity        = 0.3f;
    lightData.cbPointLight.specPow          = 8.f;
    lightData.cbPointLight.attenuation      = XMFLOAT3(0.5f, 0.25f, 1.f);
    lightData.cbPointLight.range            = 20.f;

    // Compute light direction
    //lightDir = -XMVector3Normalize(XMLoadFloat3(&m_pSpotLightSrc->GetPos()));
    //lightDir3;
    //XMStoreFloat3(&lightDir3, lightDir);

    lightData.cbSpotLight.pos               = XMFLOAT4(m_pSpotLightSrc->GetPos().x, m_pSpotLightSrc->GetPos().y, m_pSpotLightSrc->GetPos().z, 1.f);
    lightData.cbSpotLight.color             = XMFLOAT3(1.f, 1.f, 0.f);
    lightData.cbSpotLight.intensity         = 0.6f;
    lightData.cbSpotLight.specPow           = 32.f;
    lightData.cbSpotLight.attenuation       = XMFLOAT3(0.1f, 0.25f, 1.f);
    lightData.cbSpotLight.range             = 10.f;
    //lightData.cbSpotLight.dir               = lightDir3;
    lightData.cbSpotLight.dir               = XMFLOAT3(0.f, -1.f, 0.f);
    lightData.cbSpotLight.conePow           = 128.f;

    m_pRoom[0]->Create();
    m_pRoom[0]->SetPos(XMFLOAT3(0.f, 0.f, 0.f));
    m_pRoom[0]->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
    m_pRoom[0]->SetRot(XMFLOAT3(0.f, XMConvertToRadians(180.f), 0.f));

    //m_pSphere->Create();
    //m_pSphere->SetPos(XMFLOAT3(-3.f, 0.f, 0.f));
    //m_pSphere->SetScale(XMFLOAT3(0.1f, 0.1f, 0.1f));

    m_pCloth->Create();
    m_pCloth->SetPos(XMFLOAT3(0.f, 0.0f, 0.f));
    m_pCloth->SetScale(XMFLOAT3(1.0, 1.0f, 1.0f));

    m_pSoftBody->Create();
    m_pSoftBody->SetPos(XMFLOAT3(0.f, 0.0f, 0.f));
    m_pSoftBody->SetScale(XMFLOAT3(1.0, 1.0f, 1.0f));

    //LightCBuffPS* pLightData = m_pSphere->GetLightData();
    //*pLightData = lightData;

    LightCBuffPS* pLightData = m_pPointLightSrc->GetLightData();
    *pLightData = lightData;

    pLightData = m_pCloth->GetLightData();
    *pLightData = lightData;

    pLightData = m_pSoftBody->GetLightData();
    *pLightData = lightData;

    pLightData = m_pRoom[0]->GetLightData();
    *pLightData = lightData;

    m_pGrid->Create();
    m_pGrid->SetScale(XMFLOAT3(4.f, 4.f, 4.f));
}

void SceneRenderer::ReleaseDeviceDependentResources()
{
    m_pRoom[0]->Destroy();
    delete m_pRoom[0];

    //m_pSphere->Destroy();
    //delete m_pSphere;

    m_pCloth->Destroy();
    delete m_pCloth;

    m_pSoftBody->Destroy();
    delete m_pSoftBody;


    m_pDirectionalLightSrc->Destroy();
    m_pPointLightSrc->Destroy();
    m_pSpotLightSrc->Destroy();
    m_pGrid->Destroy();

    delete m_pDirectionalLightSrc;
    delete m_pPointLightSrc;
    delete m_pSpotLightSrc;
    delete m_pGrid;
}

////////////////////////////////////////////////////////////////////////////////
//                              Text
////////////////////////////////////////////////////////////////////////////////

std::wstring SceneRenderer::GetSceneText()
{
    std::wstring sceneStr = L"";

    std::wstring eyeStr         = L"Eye                 = ";
    std::wstring dirLightStr    = L"Directional Light   = ";
    std::wstring pointLightStr  = L"Point Light         = ";
    std::wstring spotLightStr   = L"Spot Light          = ";

    if (InputMode::InputModeCamera == m_inputMode)
    {
        eyeStr          = L">Eye                = ";
    }
    else if (InputMode::InputModeDirectionalLight == m_inputMode)
    {
        dirLightStr     = L">Directional Light  = ";
    }
    else if (InputMode::InputModePointLight == m_inputMode)
    {
        pointLightStr   = L">Point Light        = ";
    }
    else if (InputMode::InputModeSpotLight == m_inputMode)
    {
        spotLightStr    = L">Spot Light         = ";
    }


    DirectX::XMFLOAT4 eye = m_camera.eye;
    sceneStr += eyeStr;
    sceneStr += std::to_wstring(eye.x) + L",";
    sceneStr += std::to_wstring(eye.y) + L",";
    sceneStr += std::to_wstring(eye.z) + L"\n";

    sceneStr += dirLightStr;
    sceneStr += std::to_wstring(m_pDirectionalLightSrc->GetPos().x) + L",";
    sceneStr += std::to_wstring(m_pDirectionalLightSrc->GetPos().y) + L",";
    sceneStr += std::to_wstring(m_pDirectionalLightSrc->GetPos().z) + L"\n";

    sceneStr += pointLightStr;
    sceneStr += std::to_wstring(m_pPointLightSrc->GetPos().x) + L",";
    sceneStr += std::to_wstring(m_pPointLightSrc->GetPos().y) + L",";
    sceneStr += std::to_wstring(m_pPointLightSrc->GetPos().z) + L"\n";

    sceneStr += spotLightStr;
    sceneStr += std::to_wstring(m_pSpotLightSrc->GetPos().x) + L",";
    sceneStr += std::to_wstring(m_pSpotLightSrc->GetPos().y) + L",";
    sceneStr += std::to_wstring(m_pSpotLightSrc->GetPos().z) + L"\n";

    return sceneStr;
}

////////////////////////////////////////////////////////////////////////////////
//                              Input
////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::OnKeyUp(Windows::System::VirtualKey key)
{
    using namespace Windows::System;

    if (key == VirtualKey::Tab) 
    {
        int32_t input = static_cast<int32_t>(m_inputMode);
        input++;
        m_inputMode = (input > (int32_t)InputMode::InputModeSpotLight) ? InputMode::InputModeCamera : (InputMode)input;

        if (InputMode::InputModeCamera == m_inputMode)
        {
            OutputDebugStringA("Camera Mode\n");
        }
        else if (InputMode::InputModeDirectionalLight == m_inputMode)
        {
            OutputDebugStringA("Directional Light Mode\n");
        }
        else if (InputMode::InputModePointLight == m_inputMode)
        {
            OutputDebugStringA("Point Light Mode\n");
        }
        else if (InputMode::InputModeSpotLight == m_inputMode)
        {
            OutputDebugStringA("Spot Light Mode\n");
        }
    }
}

void SceneRenderer::OnKeyDown(Windows::System::VirtualKey key)
{
    using namespace Windows::System;

    float inc = 0.1f;

    if (key == VirtualKey::Up)
    {
        OutputDebugStringA("Up Key Pressed\n");

        if (InputMode::InputModeCamera == m_inputMode)
        {
            m_camera.eye.y += inc;
        }
        else if (InputMode::InputModeDirectionalLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pDirectionalLightSrc->GetPos();
            pos.y += inc;
            m_pDirectionalLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModePointLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pPointLightSrc->GetPos();
            pos.y += inc;
            m_pPointLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModeSpotLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pSpotLightSrc->GetPos();
            pos.y += inc;
            m_pSpotLightSrc->SetPos(pos);
        }
    }

    if (key == VirtualKey::Down)
    {
        OutputDebugStringA("Down Key Pressed\n");

        if (InputMode::InputModeCamera == m_inputMode)
        {
            m_camera.eye.y -= inc;
        }
        else if (InputMode::InputModeDirectionalLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pDirectionalLightSrc->GetPos();
            pos.y -= inc;
            m_pDirectionalLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModePointLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pPointLightSrc->GetPos();
            pos.y -= inc;
            m_pPointLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModeSpotLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pSpotLightSrc->GetPos();
            pos.y -= inc;
            m_pSpotLightSrc->SetPos(pos);
        }
    }

    if (key == VirtualKey::Right)
    {
        OutputDebugStringA("Right Key Pressed\n");

        if (InputMode::InputModeCamera == m_inputMode)
        {
            m_camera.eye.x += inc;
        }
        else if (InputMode::InputModeDirectionalLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pDirectionalLightSrc->GetPos();
            pos.x += inc;
            m_pDirectionalLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModePointLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pPointLightSrc->GetPos();
            pos.x += inc;
            m_pPointLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModeSpotLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pSpotLightSrc->GetPos();
            pos.x += inc;
            m_pSpotLightSrc->SetPos(pos);
        }
    }

    if (key == VirtualKey::Left)
    {
        OutputDebugStringA("Left Key Pressed\n");

        if (InputMode::InputModeCamera == m_inputMode)
        {
            m_camera.eye.x -= inc;
        }
        else if (InputMode::InputModeDirectionalLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pDirectionalLightSrc->GetPos();
            pos.x -= inc;
            m_pDirectionalLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModePointLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pPointLightSrc->GetPos();
            pos.x -= inc;
            m_pPointLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModeSpotLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pSpotLightSrc->GetPos();
            pos.x -= inc;
            m_pSpotLightSrc->SetPos(pos);
        }
    }

    if (key == VirtualKey::Z)
    {
        OutputDebugStringA("Z Key Pressed\n");

        if (InputMode::InputModeCamera == m_inputMode)
        {
            m_camera.eye.z += inc;
        }
        else if (InputMode::InputModeDirectionalLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pDirectionalLightSrc->GetPos();
            pos.z += inc;
            m_pDirectionalLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModePointLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pPointLightSrc->GetPos();
            pos.z += inc;
            m_pPointLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModeSpotLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pSpotLightSrc->GetPos();
            pos.z += inc;
            m_pSpotLightSrc->SetPos(pos);
        }
    }

    {
        float nudge = 0.001f;
        if (key == VirtualKey::D)   m_pSoftBody->NudgeCenterOfMass(DirectX::XMFLOAT3(-nudge, 0.f, 0.f));
        if (key == VirtualKey::F)   m_pSoftBody->NudgeCenterOfMass(DirectX::XMFLOAT3(+nudge, 0.f, 0.f));
        if (key == VirtualKey::R)   m_pSoftBody->NudgeCenterOfMass(DirectX::XMFLOAT3(0.f, 0.f, -nudge));
        if (key == VirtualKey::C)   m_pSoftBody->NudgeCenterOfMass(DirectX::XMFLOAT3(0.f, 0.f, nudge));
    }

    {
        float nudge = 0.025f;
        float limit = 3.f;
        std::vector<EdgeConstraint*> edgeConstraint = this->m_pCloth->GetEdgeConstraint();
        if (key == VirtualKey::I)
        {
            for (auto it = edgeConstraint.begin(); it != edgeConstraint.end(); ++it)
            {
                EdgeConstraint* pEdge = *it;
                pEdge->pos.z += -nudge;

                if (pEdge->pos.z < -limit) pEdge->pos.z = -limit;
            }
        }

        if (key == VirtualKey::M)
        {
            for (auto it = edgeConstraint.begin(); it != edgeConstraint.end(); ++it)
            {
                EdgeConstraint* pEdge = *it;
                pEdge->pos.z += nudge;

                if (pEdge->pos.z > limit) pEdge->pos.z = limit;
            }
        }

    }


    if (key == VirtualKey::A)
    {
        OutputDebugStringA("A Key Pressed\n");

        if (InputMode::InputModeCamera == m_inputMode)
        {
            m_camera.eye.z -= inc;
        }
        else if (InputMode::InputModeDirectionalLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pDirectionalLightSrc->GetPos();
            pos.z -= inc;
            m_pDirectionalLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModePointLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pPointLightSrc->GetPos();
            pos.z -= inc;
            m_pPointLightSrc->SetPos(pos);
        }
        else if (InputMode::InputModeSpotLight == m_inputMode)
        {
            XMFLOAT3 pos = m_pSpotLightSrc->GetPos();
            pos.z -= inc;
            m_pSpotLightSrc->SetPos(pos);
        }
    }

    UpdateObjectLights();
}

void SceneRenderer::UpdateObjectLights()
{
    // Recompute directional light vector and re-assign point light
    XMVECTOR    lightDir = XMVector3Normalize(XMLoadFloat3(&m_pDirectionalLightSrc->GetPos()));
    XMFLOAT3    lightDir3;
    XMStoreFloat3(&lightDir3, lightDir);

    // Compute light direction
    XMVECTOR    lightDirSpot = -XMVector3Normalize(XMLoadFloat3(&m_pSpotLightSrc->GetPos()));
    XMFLOAT3    lightDir3Spot;
    XMStoreFloat3(&lightDir3Spot, lightDirSpot);

    //LightCBuffPS* plightData = m_pSphere->GetLightData();
    LightCBuffPS* plightData = m_pPointLightSrc->GetLightData();

    plightData->cbDirectionalLight.dir = XMFLOAT4(
        lightDir3.x, lightDir3.y, lightDir3.z, 1.f);

    plightData->cbPointLight.pos = XMFLOAT4(
        m_pPointLightSrc->GetPos().x,
        m_pPointLightSrc->GetPos().y,
        m_pPointLightSrc->GetPos().z,
        1.f);

    plightData->cbSpotLight.pos = XMFLOAT4(
        m_pSpotLightSrc->GetPos().x,
        m_pSpotLightSrc->GetPos().y,
        m_pSpotLightSrc->GetPos().z,
        1.f);

    //plightData->cbSpotLight.dir = lightDir3Spot;
    plightData->cbSpotLight.dir = XMFLOAT3(0.f, -1.f, 0.f);

    LightCBuffPS* plightData2 = m_pCloth->GetLightData();
    *plightData2 = *plightData;

    plightData2 = m_pSoftBody->GetLightData();
    *plightData2 = *plightData;

    plightData2 = m_pRoom[0]->GetLightData();
    *plightData2 = *plightData;

    plightData2 = m_pSpotLightSrc->GetLightData();
    *plightData2 = *plightData;

    plightData2 = m_pDirectionalLightSrc->GetLightData();
    *plightData2 = *plightData;
}