#pragma once

#include <string>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

namespace Cloth
{
    // Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
    class TextRenderer
    {
    public:
        TextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(DX::StepTimer const& timer);
        void Render();
        inline void AddText(std::wstring extraText)
        {
            m_extraText = extraText;
        };

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // Resources related to text rendering.
        std::wstring                                        m_text;
        std::wstring                                        m_extraText;
        DWRITE_TEXT_METRICS                                 m_textMetrics;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_whiteBrush;
        Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>      m_stateBlock;
        Microsoft::WRL::ComPtr<IDWriteTextLayout>           m_textLayout;
        Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormat;
    };
}