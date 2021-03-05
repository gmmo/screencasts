#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace soft_body
{
    class soft_bodyMain : public DX::IDeviceNotify
    {
    public:
        soft_bodyMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~soft_bodyMain();
        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();
        void OnKeyDown(Windows::System::VirtualKey key);

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // TODO: Replace with your own content renderers.
        std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
        std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

        // Rendering loop timer.
        DX::StepTimer m_timer;
    };
}