#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderCBuff.h"
#include "FileLoad.h"
#include "SceneRenderer.h"

#define OBJECT_RENDERER_SET_VERTICES(_name, _name2)\
    if (ObjectShape::_name == objectShape)\
    {\
        m_pObjectVertices = GLibAssets::##_name2##Vertices;\
    }

#define OBJECT_RENDERER_SET_ALL_VERTICES()\
    OBJECT_RENDERER_SET_VERTICES(Sphere, sphere);\
    OBJECT_RENDERER_SET_VERTICES(Floor, floor);\
    OBJECT_RENDERER_SET_VERTICES(Torus, torus);\
    OBJECT_RENDERER_SET_VERTICES(Monkey, monkey);\
    OBJECT_RENDERER_SET_VERTICES(Boat, boat);\
    OBJECT_RENDERER_SET_VERTICES(Raptor, raptor);\
    OBJECT_RENDERER_SET_VERTICES(Teapot, teapot);\
    OBJECT_RENDERER_SET_VERTICES(Cone, cone);\
    OBJECT_RENDERER_SET_VERTICES(Cube, cube);


namespace Cloth
{
    class ObjectRenderer
    {
    public:
        ObjectRenderer::ObjectRenderer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            ObjectShape objectShape,
            FileLoad& fileUtil);

        virtual ~ObjectRenderer() {};

        virtual void Create() = 0;

        virtual void Destroy() = 0;

        virtual void Render() = 0;

        virtual void Init(
            DirectX::XMFLOAT4X4 model,
            DirectX::XMFLOAT4X4 view,
            DirectX::XMFLOAT4X4 projection) = 0;

        virtual void Update(CameraData* pCamera) = 0;

        void SetPos(DirectX::XMFLOAT3 pos) { m_pos = pos; }
        DirectX::XMFLOAT3 GetPos() { return m_pos; }

        void SetRot(DirectX::XMFLOAT3 rot) { m_rot = rot; }
        DirectX::XMFLOAT3 GetRot() { return m_rot; }

        void SetScale(DirectX::XMFLOAT3 scale) { m_scale = scale; }
        DirectX::XMFLOAT3 GetScale() { return m_scale; }

    protected:
        DirectX::XMFLOAT3                               m_pos;
        DirectX::XMFLOAT3                               m_rot;
        DirectX::XMFLOAT3                               m_scale;

        CameraData*                                     m_pCamera;
        FileLoad&                                       m_fileUtil;

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        // Direct3D resources
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_cbTransform;
        ModelViewProjDataCBuffVS                        m_cbTransformData;

        unsigned short*                                 m_pObjectIndices;
        unsigned int                                    m_objectVertexBufferSize;
        unsigned int                                    m_objectVertexCount;
        uint32                                          m_objectIndexCount;
    };
};
