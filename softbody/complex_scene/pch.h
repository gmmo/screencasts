#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#include <collection.h>
#include <ppltasks.h>

#define OUTPUT_LOG_MSG(_msg)\
    {\
        std::string msgStr(_msg);\
        msgStr += "\n";\
        OutputDebugStringA(msgStr.c_str());\
    }

#define OUTPUT_HRESULT(_hr)\
{\
    if (_hr == S_OK)\
    {\
        std::string hrStr = "hr = S_OK(" + std::to_string(_hr) + ")\n"; \
            OutputDebugStringA(hrStr.c_str()); \
    }\
    else\
    {\
        std::string hrStr = "hr = ERROR(" + std::to_string(_hr) + ")\n"; \
            OutputDebugStringA(hrStr.c_str()); \
    }\
}
