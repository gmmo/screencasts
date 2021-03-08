#include "pch.h"
#include "ObjectRenderer.h"
#include "..\Common\DirectXHelper.h"

using namespace Cloth;
using namespace DirectX;

ObjectRenderer::ObjectRenderer(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    ObjectShape objectShape,
    FileLoad& fileUtil) :
    m_deviceResources(deviceResources),
    m_fileUtil(fileUtil)
{
    m_pos = XMFLOAT3(0.f, 0.f, 0.f);
    m_rot = XMFLOAT3(0.f, 0.f, 0.f);
    m_scale = XMFLOAT3(1.f, 1.f, 1.f);

    if (ObjectShape::Sphere == objectShape)
    {
        m_pObjectIndices = GLibAssets::sphereIndices;
        m_objectIndexCount = GLibAssets::sphereIndicesCount;
        m_objectVertexBufferSize = GLibAssets::sphereVertexBufferSize;
        m_objectVertexCount = GLibAssets::sphereVertexCount;
    }
    else if (ObjectShape::Floor == objectShape)
    {
        m_pObjectIndices = GLibAssets::floorIndices;
        m_objectIndexCount = GLibAssets::floorIndicesCount;
        m_objectVertexBufferSize = GLibAssets::floorVertexBufferSize;
        m_objectVertexCount = GLibAssets::floorVertexCount;
    }
    else if (ObjectShape::Torus == objectShape)
    {
        m_pObjectIndices = GLibAssets::torusIndices;
        m_objectIndexCount = GLibAssets::torusIndicesCount;
        m_objectVertexBufferSize = GLibAssets::torusVertexBufferSize;
        m_objectVertexCount = GLibAssets::torusVertexCount;
    }
    else if (ObjectShape::Monkey == objectShape)
    {
        m_pObjectIndices = GLibAssets::monkeyIndices;
        m_objectIndexCount = GLibAssets::monkeyIndicesCount;
        m_objectVertexBufferSize = GLibAssets::monkeyVertexBufferSize;
        m_objectVertexCount = GLibAssets::monkeyVertexCount;
    }
    else if (ObjectShape::Boat == objectShape)
    {
        m_pObjectIndices = GLibAssets::boatIndices;
        m_objectIndexCount = GLibAssets::boatIndicesCount;
        m_objectVertexBufferSize = GLibAssets::boatVertexBufferSize;
        m_objectVertexCount = GLibAssets::boatVertexCount;
    }
    else if (ObjectShape::Raptor == objectShape)
    {
        m_pObjectIndices = GLibAssets::raptorIndices;
        m_objectIndexCount = GLibAssets::raptorIndicesCount;
        m_objectVertexBufferSize = GLibAssets::raptorVertexBufferSize;
        m_objectVertexCount = GLibAssets::raptorVertexCount;
    }
    else if (ObjectShape::Teapot == objectShape)
    {
        m_pObjectIndices = GLibAssets::teapotIndices;
        m_objectIndexCount = GLibAssets::teapotIndicesCount;
        m_objectVertexBufferSize = GLibAssets::teapotVertexBufferSize;
        m_objectVertexCount = GLibAssets::teapotVertexCount;
    }
    else if (ObjectShape::Cube == objectShape)
    {
        m_pObjectIndices = GLibAssets::cubeIndices;
        m_objectIndexCount = GLibAssets::cubeIndicesCount;
        m_objectVertexBufferSize = GLibAssets::cubeVertexBufferSize;
        m_objectVertexCount = GLibAssets::cubeVertexCount;
    }
    else if (ObjectShape::Cone == objectShape)
    {
        m_pObjectIndices = GLibAssets::coneIndices;
        m_objectIndexCount = GLibAssets::coneIndicesCount;
        m_objectVertexBufferSize = GLibAssets::coneVertexBufferSize;
        m_objectVertexCount = GLibAssets::coneVertexCount;
    }
    else if (ObjectShape::SphereTex == objectShape)
    {
        m_pObjectIndices = GLibAssets::sphereTexIndices;
        m_objectIndexCount = GLibAssets::sphereTexIndicesCount;
        m_objectVertexBufferSize = GLibAssets::sphereTexVertexBufferSize;
        m_objectVertexCount = GLibAssets::sphereTexVertexCount;
    }
    else if (ObjectShape::TeapotTex == objectShape)
    {
        m_pObjectIndices = GLibAssets::teapotTexIndices;
        m_objectIndexCount = GLibAssets::teapotTexIndicesCount;
        m_objectVertexBufferSize = GLibAssets::teapotTexVertexBufferSize;
        m_objectVertexCount = GLibAssets::teapotTexVertexCount;
    }
    else if (ObjectShape::PlaneTex == objectShape)
    {
        m_pObjectIndices = GLibAssets::planeTexIndices;
        m_objectIndexCount = GLibAssets::planeTexIndicesCount;
        m_objectVertexBufferSize = GLibAssets::planeTexVertexBufferSize;
        m_objectVertexCount = GLibAssets::planeTexVertexCount;
    }
    else if (ObjectShape::HiResPlaneTex == objectShape)
    {
        m_pObjectIndices = GLibAssets::hiresPlaneTexIndices;
        m_objectIndexCount = GLibAssets::hiresPlaneTexIndicesCount;
        m_objectVertexBufferSize = GLibAssets::hiresPlaneTexVertexBufferSize;
        m_objectVertexCount = GLibAssets::hiresPlaneTexVertexCount;
    }
    else if (ObjectShape::TorusTex == objectShape)
    {
        m_pObjectIndices = GLibAssets::torusTexIndices;
        m_objectIndexCount = GLibAssets::torusTexIndicesCount;
        m_objectVertexBufferSize = GLibAssets::torusTexVertexBufferSize;
        m_objectVertexCount = GLibAssets::torusTexVertexCount;
    }
    else if (ObjectShape::WallsTex == objectShape)
    {
        m_pObjectIndices = GLibAssets::wallsTexIndices;
        m_objectIndexCount = GLibAssets::wallsTexIndicesCount;
        m_objectVertexBufferSize = GLibAssets::wallsTexVertexBufferSize;
        m_objectVertexCount = GLibAssets::wallsTexVertexCount;
    }
    else if (ObjectShape::Grid == objectShape)
    {
        m_pObjectIndices = 0;
        m_objectIndexCount = 0;
        m_objectVertexBufferSize = 0;
        m_objectVertexCount = 0;
    }
    else
    {
        DX::ThrowIfFailed(-1);
    }
};

void ObjectRenderer::Init(
    DirectX::XMFLOAT4X4 model,
    DirectX::XMFLOAT4X4 view,
    DirectX::XMFLOAT4X4 projection)
{
    //m_cbTransformData.model         = model;
    //m_cbTransformData.view          = view;
    m_cbTransformData.projection    = projection;
    //m_cbTransformData.inverseWorldTranspose = model;
}

// Called once per frame updating the model and view matrices.
void ObjectRenderer::Update(CameraData* pCamera)
{
    if (pCamera)
    {
        m_pCamera = pCamera;
    }
    else
    {
        DX::ThrowIfFailed(E_FAIL);
    }

    XMVECTOR scaling            = XMVectorSet(m_scale.x, m_scale.y, m_scale.z, 1.f);
    XMVECTOR rotationOrigin     = XMVectorSet(0.f, 0.0, 0.0, 1.0f);
    XMVECTOR rotationQuaternion = XMQuaternionRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);
    XMVECTOR translation        = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 1.0f);

    XMMATRIX affineTransform    = XMMatrixAffineTransformation(scaling, rotationOrigin, rotationQuaternion, translation);
    XMMATRIX affineTransposed   = XMMatrixTranspose(affineTransform);
    XMStoreFloat4x4(&m_cbTransformData.model, affineTransposed);


    XMMATRIX affineTransform2 = XMMatrixAffineTransformation(scaling, rotationOrigin, rotationQuaternion, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
    XMMATRIX affineTransposed2 = XMMatrixTranspose(affineTransform2);
    XMStoreFloat4x4(&m_cbTransformData.inverseWorldTranspose, affineTransposed2);

    // Update camera
    XMStoreFloat4x4(&m_cbTransformData.view, XMMatrixTranspose(
        XMMatrixLookAtRH(
            XMLoadFloat4(&m_pCamera->eye),
            XMLoadFloat4(&m_pCamera->at),
            XMLoadFloat4(&m_pCamera->up))));
}

void ObjectRenderer::Destroy()
{
    m_inputLayout.Reset();
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    m_vertexShader.Reset();
    m_pixelShader.Reset();
}
