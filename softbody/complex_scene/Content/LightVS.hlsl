#include "LightStruct.h"

////////////////////////////////////////////////////////////////////////////////
// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionCBuff : register(b0)
{
    matrix cbModel;
    matrix cbView;
    matrix cbProjection;
    matrix cbInverseWorldTranspose;
};

////////////////////////////////////////////////////////////////////////////////
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos      : POSITION;
    float3 color    : COLOR0;
    float3 normal   : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos      = float4(input.pos, 1.0f);
    float4 normal4  = float4(input.normal, 1.0f);

    // Transform the vertex position into projected space.
    pos = mul(pos, cbModel);
    output.posWorld = pos.xyz;
    pos = mul(pos, cbView);
    pos = mul(pos, cbProjection);
    output.pos = pos;

    // Rotate the vertex normal to world space.
    output.normal = mul(normal4, cbInverseWorldTranspose).xyz;

    // Pass the color through without modification.
    output.color = input.color;

    return output;
}
