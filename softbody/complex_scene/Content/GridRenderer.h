#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderCBuff.h"
#include "FileLoad.h"
#include "SceneRenderer.h"
#include "ObjectRenderer.h"

namespace Cloth
{
    class GridRenderer : public ObjectRenderer
    {
    public:
        GridRenderer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            ObjectShape objectShape,
            FileLoad& fileUtil);

        ~GridRenderer() {};

        void Create();
        void Destroy();
        void Render();

        void Init(
            DirectX::XMFLOAT4X4 model,
            DirectX::XMFLOAT4X4 view,
            DirectX::XMFLOAT4X4 projection);

        void Update(CameraData* pCamera);

    private:

        GLibAssets::VertexPositionColor*    m_pObjectVertices;
        const uint32_t                      m_dim = 20;
    };
}
