#include "pch.h"
#include "GLibAssets.h"

namespace GLibAssets
{
    ////////////////////////////////////////////////////////////////////////////////
    VertexPositionColorNormalTexture wallsTexVertices[] =
    {
        #include "walls_Vertices.inl"
    };

    unsigned int wallsTexVertexBufferSize = sizeof(wallsTexVertices);
    unsigned int wallsTexVertexCount = sizeof(wallsTexVertices) / sizeof(VertexPositionColorNormalTexture);

    unsigned short wallsTexIndices[] =
    {
        #include "walls_Indices.inl"
    };
    unsigned int wallsTexIndicesCount = ARRAYSIZE(wallsTexIndices);


    ////////////////////////////////////////////////////////////////////////////////
    VertexPositionColorNormalTexture hiresPlaneTexVertices[] =
    {
        #include "hires_plane_Vertices.inl"
    };

    unsigned int hiresPlaneTexVertexBufferSize = sizeof(hiresPlaneTexVertices);
    unsigned int hiresPlaneTexVertexCount = sizeof(hiresPlaneTexVertices) / sizeof(VertexPositionColorNormalTexture);

    unsigned short hiresPlaneTexIndices[] =
    {
        #include "hires_plane_Indices.inl"
    };
    unsigned int hiresPlaneTexIndicesCount = ARRAYSIZE(hiresPlaneTexIndices);

    ////////////////////////////////////////////////////////////////////////////////
    VertexPositionColorNormalTexture torusTexVertices[] =
    {
        #include "textured_torus_Vertices.inl"
    };

    unsigned int torusTexVertexBufferSize = sizeof(torusTexVertices);
    unsigned int torusTexVertexCount = sizeof(torusTexVertices) / sizeof(VertexPositionColorNormalTexture);

    unsigned short torusTexIndices[] =
    {
        #include "textured_torus_Indices.inl"
    };
    unsigned int torusTexIndicesCount = ARRAYSIZE(torusTexIndices);

    ////////////////////////////////////////////////////////////////////////////////
    VertexPositionColorNormalTexture planeTexVertices[] =
    {
        #include "textured_plane_Vertices.inl"
    };

    unsigned int planeTexVertexBufferSize = sizeof(planeTexVertices);
    unsigned int planeTexVertexCount = sizeof(planeTexVertices) / sizeof(VertexPositionColorNormalTexture);

    unsigned short planeTexIndices[] =
    {
        #include "textured_plane_Indices.inl"
    };
    unsigned int planeTexIndicesCount = ARRAYSIZE(planeTexIndices);

    ////////////////////////////////////////////////////////////////////////////////
    VertexPositionColorNormalTexture teapotTexVertices[] =
    {
        #include "textured_teapot_Vertices.inl"
    };

    unsigned int teapotTexVertexBufferSize = sizeof(teapotTexVertices);
    unsigned int teapotTexVertexCount = sizeof(teapotTexVertices) / sizeof(VertexPositionColorNormalTexture);

    unsigned short teapotTexIndices[] =
    {
        #include "textured_teapot_Indices.inl"
    };
    unsigned int teapotTexIndicesCount = ARRAYSIZE(teapotTexIndices);

    ////////////////////////////////////////////////////////////////////////////////
    VertexPositionColorNormalTexture sphereTexVertices[] =
    {
        #include "ico_sphere_Vertices.inl"
    };

    unsigned int sphereTexVertexBufferSize = sizeof(sphereTexVertices);
    unsigned int sphereTexVertexCount = sizeof(sphereTexVertices) / sizeof(VertexPositionColorNormalTexture);

    unsigned short sphereTexIndices[] =
    {
        #include "ico_sphere_Indices.inl"
    };
    unsigned int sphereTexIndicesCount = ARRAYSIZE(sphereTexIndices);

    ////////////////////////////////////////////////////////////////////////////////
    VertexPositionColorNormal sphereVertices[] =
    {
        #include "sphereVertices.inl"
    };

    unsigned int sphereVertexBufferSize = sizeof(sphereVertices);
    unsigned int sphereVertexCount = sizeof(sphereVertices) / sizeof(VertexPositionColorNormal);

    unsigned short sphereIndices[] =
    {
        #include "sphereIndices.inl"
    };
    unsigned int sphereIndicesCount = ARRAYSIZE(sphereIndices);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal floorVertices[] =
    {
        #include "floorVertices.inl"
    };

    unsigned short floorIndices[] =
    {
        #include "floorIndices.inl"
    };

    unsigned int floorIndicesCount = ARRAYSIZE(floorIndices);
    unsigned int floorVertexBufferSize = sizeof(floorVertices);
    unsigned int floorVertexCount = sizeof(floorVertices) / sizeof(GLibAssets::VertexPositionColorNormal);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal monkeyVertices[] =
    {
        #include "monkeyVertices.inl"
    };

    unsigned short monkeyIndices[] =
    {
        #include "monkeyIndices.inl"
    };

    unsigned int monkeyIndicesCount = ARRAYSIZE(monkeyIndices);
    unsigned int monkeyVertexBufferSize = sizeof(monkeyVertices);
    unsigned int monkeyVertexCount = sizeof(monkeyVertices) / sizeof(GLibAssets::VertexPositionColorNormal);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal raptorVertices[] =
    {
        #include "raptorVertices.inl"
    };

    unsigned short raptorIndices[] =
    {
        #include "raptorIndices.inl"
    };

    unsigned int raptorIndicesCount = ARRAYSIZE(raptorIndices);
    unsigned int raptorVertexBufferSize = sizeof(raptorVertices);
    unsigned int raptorVertexCount = sizeof(raptorVertices) / sizeof(GLibAssets::VertexPositionColorNormal);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal torusVertices[] =
    {
        #include "torusVertices.inl"
    };

    unsigned short torusIndices[] =
    {
        #include "torusIndices.inl"
    };

    unsigned int torusIndicesCount = ARRAYSIZE(torusIndices);
    unsigned int torusVertexBufferSize = sizeof(torusVertices);
    unsigned int torusVertexCount = sizeof(torusVertices) / sizeof(GLibAssets::VertexPositionColorNormal);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal boatVertices[] =
    {
        #include "boatVertices.inl"
    };

    unsigned short boatIndices[] =
    {
        #include "boatIndices.inl"
    };

    unsigned int boatIndicesCount = ARRAYSIZE(boatIndices);
    unsigned int boatVertexBufferSize = sizeof(boatVertices);
    unsigned int boatVertexCount = sizeof(boatVertices) / sizeof(GLibAssets::VertexPositionColorNormal);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal teapotVertices[] =
    {
        #include "teapotVertices.inl"
    };

    unsigned short teapotIndices[] =
    {
        #include "teapotIndices.inl"
    };

    unsigned int teapotIndicesCount = ARRAYSIZE(teapotIndices);
    unsigned int teapotVertexBufferSize = sizeof(teapotVertices);
    unsigned int teapotVertexCount = sizeof(teapotVertices) / sizeof(GLibAssets::VertexPositionColorNormal);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal cubeVertices[] =
    {
        #include "cubeVertices.inl"
    };

    unsigned short cubeIndices[] =
    {
        #include "cubeIndices.inl"
    };

    unsigned int cubeIndicesCount = ARRAYSIZE(cubeIndices);
    unsigned int cubeVertexBufferSize = sizeof(cubeVertices);
    unsigned int cubeVertexCount = sizeof(cubeVertices) / sizeof(GLibAssets::VertexPositionColorNormal);

    ////////////////////////////////////////////////////////////////////////////////
    GLibAssets::VertexPositionColorNormal coneVertices[] =
    {
        #include "cone_Vertices.inl"
    };

    unsigned short coneIndices[] =
    {
        #include "cone_Indices.inl"
    };

    unsigned int coneIndicesCount = ARRAYSIZE(coneIndices);
    unsigned int coneVertexBufferSize = sizeof(coneVertices);
    unsigned int coneVertexCount = sizeof(coneVertices) / sizeof(GLibAssets::VertexPositionColorNormal);
}