#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderCBuff.h"
#include "FileLoad.h"
#include "SceneRenderer.h"
#include "ObjectRenderer.h"

namespace Cloth
{
    class FlatRenderer : public ObjectRenderer
    {
    public:
        FlatRenderer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            ObjectShape objectShape,
            FileLoad& fileUtil);

        ~FlatRenderer() {};

        void Create();
        void Destroy();
        void Render();

        void Init(
            DirectX::XMFLOAT4X4 model,
            DirectX::XMFLOAT4X4 view,
            DirectX::XMFLOAT4X4 projection);

        void Update(CameraData* pCamera);

    private:

        GLibAssets::VertexPositionColorNormal*  m_pObjectVertices;
    };
}
