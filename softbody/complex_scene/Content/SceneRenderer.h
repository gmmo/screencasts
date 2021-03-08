#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderCBuff.h"
#include "FileLoad.h"

namespace Cloth
{
    class LightRenderer;
    class FlatRenderer;
    class GridRenderer;
    class LightTexRenderer;
    class SoftBodyRenderer;

    struct CameraData
    {
        DirectX::XMFLOAT4   eye;
        DirectX::XMFLOAT4   at;
        DirectX::XMFLOAT4   up;
    };

    enum InputMode
    {
        InputModeCamera             = 0,
        InputModeDirectionalLight   = 1,
        InputModePointLight         = 2,
        InputModeSpotLight          = 3,
    };

    enum ObjectShape
    {
        Grid,
        Sphere,
        Floor,
        Torus,
        Raptor,
        Monkey,
        Boat,
        Teapot,
        Cube,
        Cone,

        SphereTex,
        TorusTex,
        PlaneTex,
        HiResPlaneTex,
        TeapotTex,

        WallsTex,
    };

    // This sample renderer instantiates a basic rendering pipeline.
    class SceneRenderer
    {
    public:
        SceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void CreateWindowSizeDependentResources();

        void CreateDeviceDependentResources();

        void ReleaseDeviceDependentResources();

        void Render();

        void Update(DX::StepTimer const& timer);

        void OnKeyDown(Windows::System::VirtualKey key);

        void OnKeyUp(Windows::System::VirtualKey key);

        std::wstring GetSceneText();

        static void CreateVertexNormals(
            unsigned short*,
            uint32,
            GLibAssets::VertexPositionColorNormal*,
            uint32_t);

        static void CreateVertexNormals(
            unsigned short*,
            uint32,
            GLibAssets::VertexPositionColorNormalTexture*,
            uint32_t);

        void UpdateObjectLights();

    protected:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>    m_deviceResources;

        CameraData                              m_camera;
        float                                   m_degreesPerSecond;
        FileLoad                                m_fileUtil;

        //LightRenderer*                          m_pSphere;
        SoftBodyRenderer*                       m_pCloth;
        SoftBodyRenderer*                       m_pSoftBody;

        LightRenderer*                          m_pDirectionalLightSrc;
        LightRenderer*                          m_pPointLightSrc;
        LightRenderer*                          m_pSpotLightSrc;

        LightTexRenderer*                       m_pRoom[1];

        GridRenderer*                           m_pGrid;

        InputMode                               m_inputMode;
    };
}

