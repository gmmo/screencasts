#include "pch.h"
#include "ParticleSystem.h"
#include <string>

using namespace DirectX;

namespace soft_body
{
    ParticleSystem::~ParticleSystem()
    {
        for (auto it = m_stickConstraint.begin(); it != m_stickConstraint.end(); ++it)
        {
            StickConstraint* pStick = *it;
            delete pStick;
        }

        m_stickConstraint.clear();
    }

    void ParticleSystem::Init()
    {
        m_vGravity = XMFLOAT3(0.f, -8.f, 0.f);

        const float     length = 6.0f;
        const float     size = length / (static_cast<float>(PARTICLES_DIM) - 1.f);

        float height = 2.f;
        uint32_t ii = 0;
        for (uint32_t zz = 0; zz < PARTICLES_DIM; zz++)
        {
            for (uint32_t xx = 0; xx < PARTICLES_DIM; xx++)
            {
                m_x[ii] = XMFLOAT3(
                    static_cast<float>(xx) * size - length / 2,
                    height,
                    static_cast<float>(zz) * size - length / 2);

                m_oldx[ii] = m_x[ii];
                ii++;
            }
        }


        // build stick constraints
        m_restLength = size;

        // save edge constraint
        for (uint32_t xx = 0; xx < PARTICLES_DIM; xx++)
        {
            m_edgeConstraint[xx] = m_x[xx];
        }

        // build stick constraints (horizontal sticks)
        for (uint32_t zz = 0; zz < PARTICLES_DIM; zz++)
        {
            for (uint32_t xx = 0; xx < PARTICLES_DIM - 1; xx++)
            {
                uint32_t particleA = xx + PARTICLES_DIM * zz;
                uint32_t particleB = particleA + 1;

                StickConstraint* pStick = new StickConstraint(particleA, particleB);

                m_stickConstraint.push_back(pStick);
            }
        }

        // build stick constraints (vertical sticks)
        for (uint32_t xx = 0; xx < PARTICLES_DIM; xx++)
        {
            for (uint32_t zz = 0; zz < PARTICLES_DIM - 1; zz++)
            {
                uint32_t p0 = xx + PARTICLES_DIM * zz;
                uint32_t p1 = p0 + PARTICLES_DIM;

                StickConstraint* pStick = new StickConstraint(p0, p1);

                m_stickConstraint.push_back(pStick);
            }
        }
    }

    XMFLOAT3& ParticleSystem::GetParticlePos(uint32_t ii)
    {
        return m_x[ii];
    }


    // Verlet integration step
    void ParticleSystem::Verlet(float fTimeStep)
    {
        float f1 = 0.99f;
        float f2 = 0.99f;

        for (int i = 0; i < NUM_PARTICLES; i++)
        {
            XMVECTOR x = XMLoadFloat3(&m_x[i]);
            XMVECTOR temp = x;
            XMVECTOR oldx = XMLoadFloat3(&m_oldx[i]);
            XMVECTOR a = XMLoadFloat3(&m_a[i]);
            x += f1 * x - f2 * oldx + a * fTimeStep * fTimeStep;
            XMStoreFloat3(&m_x[i], x);
            XMStoreFloat3(&m_oldx[i], temp);
        }
    }

    // This function should accumulate forces for each particle
    void ParticleSystem::AccumulateForces()
    {
        // All particles are influenced by gravity
        for (int i = 0; i < NUM_PARTICLES; i++)
        {
            m_a[i] = m_vGravity;
        }
    }

    // Here constraints should be satisfied
    void ParticleSystem::SatisfyConstraints()
    {
        // Satisfy edge constraint (C1)
        for (uint32_t xx = 0; xx < PARTICLES_DIM; xx++)
            //for (uint32_t xx = 0; xx < 1; xx++)
        {
            m_x[xx] = m_edgeConstraint[xx];
        }

        for (int j = 0; j < NUM_ITERATIONS; j++)
        {
            // Satifying C2 (stick constraint)
            for (auto it = m_stickConstraint.begin(); it != m_stickConstraint.end(); ++it)
            {
                //Constraint& c = m_constraints[i];
                //Vector3& x1 = m_x[c.particleA];
                //Vector3& x2 = m_x[c.particleB];

                StickConstraint* pStick = *it;
                XMFLOAT3& x1 = m_x[pStick->particleA];
                XMFLOAT3& x2 = m_x[pStick->particleB];

                //Vector3 delta = x2 - x1;
                //float deltalength = sqrt(delta*delta);
                //float diff = (deltalength - c.restlength) / deltalength;
                //x1 += delta*0.5*diff;
                //x2 -= delta*0.5*diff;

                XMFLOAT3 delta;
                delta.x = x2.x - x1.x;
                delta.y = x2.y - x1.y;
                delta.z = x2.z - x1.z;

                float deltalength = (float)sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
                float diff = (deltalength - m_restLength) / deltalength;

                x1.x += delta.x * 0.5f * diff;
                x1.y += delta.y * 0.5f * diff;
                x1.z += delta.z * 0.5f * diff;

                x2.x -= delta.x * 0.5f * diff;
                x2.y -= delta.y * 0.5f * diff;
                x2.z -= delta.z * 0.5f * diff;
            }

            // Satisfy sphere constaint (C3)
            for (uint32_t ii = 0; ii < NUM_PARTICLES; ii++)
            {
                // Calculate P'' = Center + ContactNormal*Radius
                float       bias = 0.01f;
                float       sphereRadius = 0.5f + bias;

                XMFLOAT3    sphereCenter = XMFLOAT3(0.f, 0.f, 0.f);
                XMFLOAT3& p = m_x[ii];
                XMFLOAT3    contactNormal;

                contactNormal.x = p.x - sphereCenter.x;
                contactNormal.y = p.y - sphereCenter.y;
                contactNormal.z = p.z - sphereCenter.z;

                float deltalength = sqrtf(
                    contactNormal.x * contactNormal.x +
                    contactNormal.y * contactNormal.y +
                    contactNormal.z * contactNormal.z);

                // We collided
                if (deltalength < sphereRadius)
                {
                    // Normalize
                    contactNormal.x /= deltalength;
                    contactNormal.y /= deltalength;
                    contactNormal.z /= deltalength;

                    // Project the point to the surface of the sphere
                    m_x[ii].x = sphereCenter.x + (sphereRadius * contactNormal.x);
                    m_x[ii].y = sphereCenter.y + (sphereRadius * contactNormal.y);
                    m_x[ii].z = sphereCenter.z + (sphereRadius * contactNormal.z);
                }
            }

        }   //for (int j = 0; j<NUM_ITERATIONS; j++)
    }

    void ParticleSystem::Update(DX::StepTimer const& timer)
    {
        // Get the time step
        float timeStep = static_cast<float>(timer.GetElapsedSeconds());
        std::wstring   timeStepStr = std::to_wstring(timeStep) + L"\n";
        OutputDebugString(timeStepStr.c_str());

        AccumulateForces();
        Verlet(timeStep);
        SatisfyConstraints();
    }
}