#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderCBuff.h"
#include "SceneRenderer.h"
#include "LightTexRenderer.h"
#include <vector>

namespace Cloth
{
    struct StickConstraint
    {
        StickConstraint(uint32_t pA, uint32_t pB, float restLength)
        {
            this->particleA     = pA;
            this->particleB     = pB;
            this->restLength    = restLength;
        }

        uint32_t    particleA;
        uint32_t    particleB;
        float       restLength;
    };

    struct StickConstraintCenterOfMass
    {
        StickConstraintCenterOfMass(uint32_t p, float restLength)
        {
            this->particle      = p;
            this->restLength    = restLength;
        }

        uint32_t    particle;
        float       restLength;
    };


    struct EdgeConstraint
    {
        EdgeConstraint(DirectX::XMFLOAT3 pos, uint32_t index)
        {
            this->pos       = pos;
            this->index     = index;
        }

        DirectX::XMFLOAT3   pos;
        uint32_t            index;
    };

    class SoftBodyRenderer : public LightTexRenderer
    {
    public:
        SoftBodyRenderer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources,
            ObjectShape objectShape,
            FileLoad& fileUtil,
            const std::wstring& filename = L"Assets\\checkerboard.dds",
            DirectX::XMFLOAT3 worldTranslation = DirectX::XMFLOAT3(0.f, 0.f, 0.f));

        ~SoftBodyRenderer() {};

        void Create();
        void Destroy();
        void Render();

        void Init(
            XMFLOAT4X4 model,
            XMFLOAT4X4 view,
            XMFLOAT4X4 projection);

        std::vector<StickConstraint*> GetStickConstraint() { return m_stickConstraint; }

        std::vector<EdgeConstraint*> GetEdgeConstraint() { return m_edgeConstraint; }


        void Update(CameraData* pCamera);
        void Update(DX::StepTimer const& timer);
        void NudgeCenterOfMass(DirectX::XMFLOAT3 nudge);

        DirectX::XMFLOAT3 GetCenterOfMass()
        {
            return m_centerOfMass;
        }

        void SetCollidingSphere(DirectX::XMFLOAT3 sphereCenter, float radius)
        {
            m_sphereCenter  = sphereCenter;
            m_radius        = radius;
        }

    private:
        void UpdateVertices();
        void Verlet(float fTimeStep);
        void SatisfyConstraints();
        void AccumulateForces();
        StickConstraint* SoftBodyRenderer::FindConstraint(unsigned short pA, unsigned short pB);

    private:
        const uint32_t                                      m_numIterations = 8;
        uint32_t                                            m_numParticles;
        std::vector<StickConstraintCenterOfMass*>           m_stickConstraintCenterOfMass;
        std::vector<StickConstraint*>                       m_stickConstraint;
        std::vector<EdgeConstraint*>                        m_edgeConstraint;

        GLibAssets::VertexPositionColorNormalTexture*       m_x;                    // Current positions
        DirectX::XMFLOAT3*                                  m_oldx;                 // Previous positions
        DirectX::XMFLOAT3*                                  m_a;                    // Force accumulators
        DirectX::XMFLOAT3                                   m_vGravity;             // Gravity
        DirectX::XMFLOAT3                                   m_centerOfMass;         // Object's center of mass
        DirectX::XMFLOAT3                                   m_centerOfMassOld;      // Object's center of mass

        // Colliding objects
        ObjectShape                                         m_objectShape;
        DirectX::XMFLOAT3                                   m_sphereCenter;
        float                                               m_radius;
    };
}

