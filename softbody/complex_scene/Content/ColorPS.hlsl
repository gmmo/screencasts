#include "ColorStruct.hlsl"

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}
