#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace GLibAssets
{
    struct VertexPositionColor
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 color;
    };

    struct VertexPositionColorNormal
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 color;
        DirectX::XMFLOAT3 normal;
    };

    struct VertexPositionColorNormalTexture
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 color;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 tex;
    };

    // Color & Texture
    extern GLibAssets::VertexPositionColorNormalTexture wallsTexVertices[];
    extern unsigned int wallsTexVertexBufferSize;
    extern unsigned int wallsTexVertexCount;
    extern unsigned short wallsTexIndices[];
    extern unsigned int wallsTexIndicesCount;


    extern GLibAssets::VertexPositionColorNormalTexture teapotTexVertices[];
    extern unsigned int teapotTexVertexBufferSize;
    extern unsigned int teapotTexVertexCount;
    extern unsigned short teapotTexIndices[];
    extern unsigned int teapotTexIndicesCount;


    extern GLibAssets::VertexPositionColorNormalTexture sphereTexVertices[];
    extern unsigned int sphereTexVertexBufferSize;
    extern unsigned int sphereTexVertexCount;
    extern unsigned short sphereTexIndices[];
    extern unsigned int sphereTexIndicesCount;

    extern GLibAssets::VertexPositionColorNormalTexture torusTexVertices[];
    extern unsigned int torusTexVertexBufferSize;
    extern unsigned int torusTexVertexCount;
    extern unsigned short torusTexIndices[];
    extern unsigned int torusTexIndicesCount;

    extern GLibAssets::VertexPositionColorNormalTexture planeTexVertices[];
    extern unsigned int planeTexVertexBufferSize;
    extern unsigned int planeTexVertexCount;
    extern unsigned short planeTexIndices[];
    extern unsigned int planeTexIndicesCount;

    extern GLibAssets::VertexPositionColorNormalTexture hiresPlaneTexVertices[];
    extern unsigned int hiresPlaneTexVertexBufferSize;
    extern unsigned int hiresPlaneTexVertexCount;
    extern unsigned short hiresPlaneTexIndices[];
    extern unsigned int hiresPlaneTexIndicesCount;

    // Color
    extern GLibAssets::VertexPositionColorNormal sphereVertices[];
    extern unsigned int sphereVertexBufferSize;
    extern unsigned int sphereVertexCount;
    extern unsigned short sphereIndices[];
    extern unsigned int sphereIndicesCount;

    extern GLibAssets::VertexPositionColorNormal floorVertices[];
    extern unsigned int floorVertexBufferSize;
    extern unsigned int floorVertexCount;
    extern unsigned short floorIndices[];
    extern unsigned int floorIndicesCount;

    extern GLibAssets::VertexPositionColorNormal monkeyVertices[];
    extern unsigned int monkeyVertexBufferSize;
    extern unsigned int monkeyVertexCount;
    extern unsigned short monkeyIndices[];
    extern unsigned int monkeyIndicesCount;

    extern GLibAssets::VertexPositionColorNormal raptorVertices[];
    extern unsigned int raptorVertexBufferSize;
    extern unsigned int raptorVertexCount;
    extern unsigned short raptorIndices[];
    extern unsigned int raptorIndicesCount;

    extern GLibAssets::VertexPositionColorNormal torusVertices[];
    extern unsigned int torusVertexBufferSize;
    extern unsigned int torusVertexCount;
    extern unsigned short torusIndices[];
    extern unsigned int torusIndicesCount;

    extern GLibAssets::VertexPositionColorNormal boatVertices[];
    extern unsigned int boatVertexBufferSize;
    extern unsigned int boatVertexCount;
    extern unsigned short boatIndices[];
    extern unsigned int boatIndicesCount;

    extern GLibAssets::VertexPositionColorNormal teapotVertices[];
    extern unsigned int teapotVertexBufferSize;
    extern unsigned int teapotVertexCount;
    extern unsigned short teapotIndices[];
    extern unsigned int teapotIndicesCount;

    extern GLibAssets::VertexPositionColorNormal cubeVertices[];
    extern unsigned int cubeVertexBufferSize;
    extern unsigned int cubeVertexCount;
    extern unsigned short cubeIndices[];
    extern unsigned int cubeIndicesCount;

    extern GLibAssets::VertexPositionColorNormal coneVertices[];
    extern unsigned int coneVertexBufferSize;
    extern unsigned int coneVertexCount;
    extern unsigned short coneIndices[];
    extern unsigned int coneIndicesCount;
}
