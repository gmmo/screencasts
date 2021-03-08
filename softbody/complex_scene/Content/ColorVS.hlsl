#include "ColorStruct.hlsl"

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionCBuff : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    matrix inverseWorldTranspose;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos      : POSITION;
    float3 color    : COLOR0;
    float3 normal   : NORMAL;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);
    float4 normal4 = float4(input.normal, 1.0f);

    // friendly reminder (3 matrices, 4 spaces)
    // posModel * model_matrix      = world coordinates
    // posWorld * view_matrix       = view coordinates
    // posView  * projection_matrix = screen coordinates
    
    // Transform the vertex position into projected space.
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.pos = pos;

    //normal4 = mul(normal4, model);
    output.normal = (float3)normal4;

    // Pass the color through without modification.
    output.color = input.color;

    return output;
}
