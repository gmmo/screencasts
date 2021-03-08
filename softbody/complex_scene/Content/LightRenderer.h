#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderCBuff.h"
#include "SceneRenderer.h"
#include "ObjectRenderer.h"

namespace Cloth
{
    class LightRenderer : public ObjectRenderer
    {
    public:
        LightRenderer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            ObjectShape objectShape,
            FileLoad& fileUtil);

        ~LightRenderer() {};

        void Create();
        void Destroy();
        void Render();

        void Init(
            XMFLOAT4X4 model,
            XMFLOAT4X4 view,
            XMFLOAT4X4 projection);

        void Update(CameraData* pCamera);

        LightCBuffPS*     GetLightData()          { return &m_LightData; };

    private:
        void UpdateLightCBuff();
        void UpdateMaterialCBuff();

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_cbMaterial;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_cbLight;

        GLibAssets::VertexPositionColorNormal*          m_pObjectVertices;
        LightCBuffPS                                    m_LightData;
    };
}

