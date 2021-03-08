#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "ParticleSystem.h"

namespace soft_body
{
    // This sample renderer instantiates a basic rendering pipeline.
    class Sample3DSceneRenderer
    {
    public:
        Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void CreateCloth();
        void CreateSphere();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(DX::StepTimer const& timer);
        void UpdateCloth(DX::StepTimer const& timer);
        void Render();
        void RenderCloth();
        void RenderSphere();
        void StartTracking();
        void TrackingUpdate(float positionX);
        void StopTracking();
        bool IsTracking() { return m_tracking; }
        void OnKeyDown(Windows::System::VirtualKey key);

    private:
        void RotateCloth(float radians);
        void TranslateSphere();
        std::vector<byte> Sample3DSceneRenderer::ReadFileSync(const std::wstring& filename);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // Direct3D resources for cloth geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayoutCloth;
        Microsoft::WRL::ComPtr<ID3D11Buffer>  m_vertexBufferCloth;
        Microsoft::WRL::ComPtr<ID3D11Buffer>  m_indexBufferCloth;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShaderCloth;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShaderCloth;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBufferCloth;

        // added
        VertexPositionColor* m_pClothVertices;
        size_t m_clothVerticesSize;
        uint32 m_clothIndexCount;

        // Direct3D resources for sphere geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayoutSphere;
        Microsoft::WRL::ComPtr<ID3D11Buffer>  m_vertexBufferSphere;
        Microsoft::WRL::ComPtr<ID3D11Buffer>  m_indexBufferSphere;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShaderSphere;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShaderSphere;
        Microsoft::WRL::ComPtr<ID3D11Buffer>  m_constantBufferSphere;

        // added
        VertexPositionColor* m_pSphereVertices;
        size_t m_sphereVerticesSize;
        uint32 m_sphereIndexCount;
        ParticleSystem m_particleSystem;

        // System resources for cube geometry.
        ModelViewProjectionConstantBuffer m_constantBufferDataCloth;
        ModelViewProjectionConstantBuffer m_constantBufferDataSphere;
        std::vector<byte> m_data;

        // Variables used with the rendering loop.
        bool m_loadingComplete;
        float m_degreesPerSecond;
        bool m_tracking;
        float m_X, m_Y, m_Z;
    };
}

