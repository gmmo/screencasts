#pragma once

#include "Data\GLibAssets.h"

namespace Cloth
{
    // CBuffVS = Constant buffer for vertex shader
    // CBuffPS = Constant buffer for pixel shader

    // Constant buffer used to send MVP matrices to the vertex shader.
    struct ModelViewProjDataCBuffVS
    {
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
        DirectX::XMFLOAT4X4 inverseWorldTranspose;
    };

    struct MaterialCBuffPS
    {
        DirectX::XMFLOAT4   ambient;
        DirectX::XMFLOAT4   diffuse;
        DirectX::XMFLOAT4   specular;
    };

    struct DirectionalLight
    {
        DirectX::XMFLOAT4   dir;
        DirectX::XMFLOAT3   color;
        float               specPow;
        float               intensity;
        DirectX::XMFLOAT3   pad;
    };

    struct PointLight
    {
        DirectX::XMFLOAT4   pos;            // world coordinates (Position where the light illuminate this object and not the object position)

        DirectX::XMFLOAT3   color;
        float               specPow;

        DirectX::XMFLOAT3   attenuation;    //x = a0, y=a1, z=a2
        float               range;

        float               intensity;      // no need to be in the shader, left for clarity
        DirectX::XMFLOAT3   pad;
    };

    struct SpotLight
    {
        DirectX::XMFLOAT4   pos;

        DirectX::XMFLOAT3   color;
        float               specPow;

        DirectX::XMFLOAT3   attenuation;
        float               range;

        float               intensity;  // no need to be in the shader, left for clarity
        DirectX::XMFLOAT3   pad;

        DirectX::XMFLOAT3   dir;        // Direction of the spot light
        float               conePow;    // Size of the cone of illumination, controls the power
                                        // function (spotDirection.LightDirection) ^ conePow
                                        // spotDirection = spot light direction
                                        // LightDirection = vector between light source and vertex
    };

    struct LightCBuffPS
    {
        DirectX::XMFLOAT4   cbEyePos;
        DirectionalLight    cbDirectionalLight;
        PointLight          cbPointLight;
        SpotLight           cbSpotLight;
    };
}