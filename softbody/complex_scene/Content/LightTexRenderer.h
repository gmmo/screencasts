#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderCBuff.h"
#include "SceneRenderer.h"
#include "ObjectRenderer.h"

namespace Cloth
{
    class LightTexRenderer : public ObjectRenderer
    {
    public:
        LightTexRenderer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            ObjectShape objectShape,
            FileLoad& fileUtil,
            const std::wstring& filename = L"Assets\\checkerboard.dds");

        ~LightTexRenderer() {};

        void Create();
        void Destroy();
        void Render();

        void Init(
            XMFLOAT4X4 model,
            XMFLOAT4X4 view,
            XMFLOAT4X4 projection);

        void Update(CameraData* pCamera);

        LightCBuffPS*     GetLightData()          { return &m_LightData; };

    protected:
        void UpdateLightCBuff();
        void UpdateMaterialCBuff();

    protected:
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_cbMaterial;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_cbLight;
        Microsoft::WRL::ComPtr<ID3D11Resource>              m_texture;      // 2d texture
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_textureView;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_textureSamplerLinear;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState>       m_NoCullRS;

        GLibAssets::VertexPositionColorNormalTexture*       m_pObjectVertices;
        LightCBuffPS                                        m_LightData;
    };
}

