#include "LightTexStruct.h"

////////////////////////////////////////////////////////////////////////////////
Texture2D       texDiffuse      : register(t8);
SamplerState    samplerLinear   : register(s9);

////////////////////////////////////////////////////////////////////////////////
cbuffer MaterialCBuffPS : register(b3)
{
    float4  cbAmbient;
    float4  cbDiffuse;
    float4  cbSpecular;
};

////////////////////////////////////////////////////////////////////////////////
struct DirectionalLight
{
    float4  dir;

    float3  color;
    float   specPow;

    float   intensity;
    float3  pad;
};

struct PointLight
{
    float4  pos;

    float3  color;
    float   specPow;

    float3  attenuation;
    float   range;

    float   intensity;
    float3  pad;
};

struct SpotLight
{
    float4  pos;

    float3  color;
    float   specPow;

    float3  attenuation;
    float   range;

    float   intensity;
    float3  pad;

    float3  dir;        // Direction of the spot light
    float   conePow;    // Size of the cone of illumination, controls the power
};

cbuffer LightCBuffPS : register(b4)
{
    float4              cbEyePos;
    DirectionalLight    cbDirectionalLight;
    PointLight          cbPointLight;
    SpotLight           cbSpotLight;
}

////////////////////////////////////////////////////////////////////////////////
void ComputeDirectionalLight(
    out float4  ambient,
    out float4  diffuse,
    out float4  specular, 
    float3      normal,
    float3      eyeVector)
{
    // Ambient = (MatAmbient color)*(light color)
    float4 lightColor = float4(cbDirectionalLight.color, 0.0f);
    ambient = cbAmbient * lightColor;

    // Diffuse = (MatDiffuse color)*max(dot, 0)*(light color)
    specular    = (float4)0.f;
    diffuse     = (float4)0.f;
    float3 lightDir = -normalize((cbDirectionalLight.dir.xyz));
    float   kDiffuse = dot(lightDir, normal);
    if (kDiffuse > 0)
    {
        diffuse = (kDiffuse*cbDiffuse) * lightColor;
        float3 lightReflect = reflect(-lightDir, normal);

        // kSpecular = max (EyeVector.Reflected, 0)^specular power
        // Note: Eye vector per vertex and not per pixel
        float kSpecular = pow(max(dot(eyeVector, lightReflect), 0.f), cbDirectionalLight.specPow);
        specular = (kSpecular*cbSpecular) * lightColor;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ComputePointLight(
    out float4  ambient,
    out float4  diffuse,
    out float4  specular,
    float3      normal,
    float3      eyeVector,
    float3      vertexWorld)
{
    // zero out color components
    specular    = (float4)0.f;
    diffuse     = (float4)0.f;
    ambient     = (float4)0.f;

    // Computer vector from light source to surface
    float3 pointLightDir = cbPointLight.pos.xyz - vertexWorld;

    // distance to the triangle surface
    float distance = length(pointLightDir);

    // turn off light is passes the range
    if (distance > cbPointLight.range)
        return;

    // Ambient = (MatAmbient color)*(light color)
    float4 lightColor = float4(cbPointLight.color, 0.0f);
    ambient = cbAmbient * lightColor;

    // Diffuse = (MatDiffuse color)*max(dot, 0)*(light color)
    float3 lightDir     = -normalize(pointLightDir);
    float   kDiffuse    = dot(lightDir, normal);
    if (kDiffuse > 0)
    {
        diffuse = (kDiffuse*cbDiffuse) * lightColor;
        float3 lightReflect = reflect(-lightDir, normal);

        // kSpecular = max (EyeVector.Reflected, 0)^specular power
        // Note: Eye vector per vertex and not per pixel
        float kSpecular = pow(max(dot(eyeVector, lightReflect), 0.f), cbPointLight.specPow);
        specular = (kSpecular*cbPointLight.specPow) * lightColor;

        // Attenuate components
        float3  distanceFact = float3(1, distance, distance*distance);
        float   attScaling   = 1.0f/dot(cbPointLight.attenuation, distanceFact);

        diffuse     *= attScaling;
        specular    *= attScaling;
    }
}

////////////////////////////////////////////////////////////////////////////////
void ComputeSpotLight(
    out float4  ambient,
    out float4  diffuse,
    out float4  specular,
    float3      normal,
    float3      eyeVector,
    float3      vertexWorld)
{
    // zero out color components
    specular = (float4)0.f;
    diffuse = (float4)0.f;
    ambient = (float4)0.f;

    // Computer vector from light source to surface
    float3 spotLightDir = cbSpotLight.pos.xyz - vertexWorld;

    // distance to the triangle surface
    float distance = length(spotLightDir);

    // turn off light if it passes the range
    if (distance > cbSpotLight.range)
        return;

    // Diffuse = (MatDiffuse color)*max(dot, 0)*(light color)
    float3 lightDir = -normalize(spotLightDir);
    float   kDiffuse = dot(lightDir, normal);
    if (kDiffuse > 0)
    {
        // Ambient = (MatAmbient color)*(light color)
        float4 lightColor = float4(cbSpotLight.color, 0.0f);
        ambient = cbAmbient * lightColor;

        diffuse = (kDiffuse*cbDiffuse) * lightColor;
        float3 lightReflect = reflect(-lightDir, normal);

        // kSpecular = max (EyeVector.Reflected, 0)^specular power
        // Note: Eye vector per vertex and not per pixel
        float kSpecular = pow(max(dot(eyeVector, lightReflect), 0.f), cbSpotLight.specPow);
        specular = (kSpecular*cbSpotLight.specPow) * lightColor;

        // Calculate the cone of illumination
        // Angle between the spot light direction and the light vector
        float coneAngle = dot(lightDir, cbSpotLight.dir);
        float kSpot = pow(max(coneAngle, 0.f), cbSpotLight.conePow);

        // Attenuate components
        float3  distanceFact = float3(1, distance, distance*distance);
        float   attScaling = 1.0f / dot(cbSpotLight.attenuation, distanceFact);

        ambient *= kSpot;
        diffuse *= attScaling * kSpot;
        specular *= attScaling * kSpot;
    }
}

////////////////////////////////////////////////////////////////////////////////
float4 main(PixelShaderInput input) : SV_TARGET
{
    float4  ambientAcc  = (float4)0.f;
    float4  diffuseAcc  = (float4)0.f;
    float4  specularAcc = (float4)0.f;


    float4  ambient;
    float4  diffuse;
    float4  specular;

    // Interpolating normal can unnormalize it
    input.normal = normalize(input.normal);

    float3 vertexWorld = input.posWorld.xyz;
    float3 eyeVector = -normalize(cbEyePos.xyz - vertexWorld);

    ComputeDirectionalLight(ambient, diffuse, specular, input.normal, eyeVector);
    ambientAcc  += ambient;
    diffuseAcc  += diffuse;
    specularAcc += specular;

    ComputePointLight(ambient, diffuse, specular, input.normal, eyeVector, vertexWorld);
    ambientAcc += ambient;
    diffuseAcc += diffuse;
    specularAcc += specular;

    ComputeSpotLight(ambient, diffuse, specular, input.normal, eyeVector, vertexWorld);
    ambientAcc += ambient;
    diffuseAcc += diffuse;
    specularAcc += specular;

    float4 textureColor = texDiffuse.Sample(samplerLinear, input.tex);

#if 1
    float4 color = textureColor*(ambientAcc + diffuseAcc) + specularAcc;
#else
    float4 color;
    color.x = 1.f;
    color.y = 0.f;
    color.z = 1.f;
    color.w = 1.f;
#endif

    return color;
}
