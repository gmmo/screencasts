#pragma once
#pragma once

#include "..\Common\StepTimer.h"
#include "..\Common\DirectXHelper.h"
#include <vector>

// Sample code for physics simulation
namespace soft_body
{
    const uint32_t                      PARTICLES_DIM = 32;
    const uint32_t                      NUM_PARTICLES = PARTICLES_DIM * PARTICLES_DIM;
    const uint32_t                      NUM_ITERATIONS = 16;

    struct ParticleSystem
    {
        struct StickConstraint
        {
            StickConstraint(uint32_t pA, uint32_t pB)
            {
                this->particleA = pA;
                this->particleB = pB;
            }

            uint32_t    particleA;
            uint32_t    particleB;
        };

    public:
        ParticleSystem() {};
        ~ParticleSystem();

    private:
        DirectX::XMFLOAT3 m_x[NUM_PARTICLES];       // Current positions
        DirectX::XMFLOAT3 m_oldx[NUM_PARTICLES];    // Previous positions
        DirectX::XMFLOAT3 m_a[NUM_PARTICLES];       // Force accumulators
        DirectX::XMFLOAT3 m_vGravity;               // Gravity

        DirectX::XMFLOAT3               m_edgeConstraint[PARTICLES_DIM];
        std::vector<StickConstraint*>   m_stickConstraint;
        float                           m_restLength;

    public:
        void Update(DX::StepTimer const& timer);
        void Init();
        DirectX::XMFLOAT3& GetParticlePos(uint32_t ii);
        DirectX::XMFLOAT3* GetEdge() { return m_edgeConstraint; }
    private:
        void Verlet(float fTimeStep);
        void SatisfyConstraints();
        void AccumulateForces();
    };
}