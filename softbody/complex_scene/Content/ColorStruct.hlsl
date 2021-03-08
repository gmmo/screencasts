// DO NOT compile this code with the shader compiler
// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos      : SV_POSITION;
    float3 color    : COLOR0;
    float3 normal   : NORMAL;
};
