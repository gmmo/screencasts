#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\SceneRenderer.h"
#include "Content\TextRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace Cloth
{
    class ClothMain : public DX::IDeviceNotify
    {
    public:
        ClothMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~ClothMain();
        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();
        void OnKeyDown(Windows::System::VirtualKey key);
        void OnKeyUp(Windows::System::VirtualKey key);

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // TODO: Replace with your own content renderers.
        std::unique_ptr<SceneRenderer> m_sceneRenderer;
        std::unique_ptr<TextRenderer> m_fpsTextRenderer;

        // Rendering loop timer.
        DX::StepTimer m_timer;
    };
}