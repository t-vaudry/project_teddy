#include "stdafx.h"
#include "ShapeGenerator.h"

#include <CatmullRom.h>

Shape ShapeGenerator::GenerateCube(glm::vec3 color, glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate)
{
    Shape cube;

    glm::vec3 vertices[] = {
        glm::vec3(-1.0f, -1.0f, -1.0f), // 0
        glm::vec3(-1.0f, +1.0f, -1.0f), // 1
        glm::vec3(-1.0f, +1.0f, +1.0f), // 2
        glm::vec3(+1.0f, +1.0f, +1.0f), // 3
        glm::vec3(+1.0f, -1.0f, +1.0f), // 4
        glm::vec3(+1.0f, -1.0f, -1.0f), // 5
        glm::vec3(+1.0f, +1.0f, -1.0f), // 6
        glm::vec3(-1.0f, -1.0f, +1.0f), // 7
    };

    glm::vec3 normals[] = {
        glm::vec3(0.0f, +1.0f, 0.0f), // 0
        glm::vec3(0.0f, -1.0f, 0.0f), // 1
        glm::vec3(+1.0f, 0.0f, 0.0f), // 2
        glm::vec3(-1.0f, 0.0f, 0.0f), // 3
        glm::vec3(0.0f, 0.0f, +1.0f), // 4
        glm::vec3(0.0f, 0.0f, -1.0f)  // 5
    };

    Vertex vertexData[] = {
        // FRONT
        Vertex(vertices[0], color, normals[5]),
        Vertex(vertices[6], color, normals[5]),
        Vertex(vertices[1], color, normals[5]),
        Vertex(vertices[0], color, normals[5]),
        Vertex(vertices[6], color, normals[5]),
        Vertex(vertices[5], color, normals[5]),

        // BACK
        Vertex(vertices[7], color, normals[4]),
        Vertex(vertices[4], color, normals[4]),
        Vertex(vertices[2], color, normals[4]),
        Vertex(vertices[3], color, normals[4]),
        Vertex(vertices[4], color, normals[4]),
        Vertex(vertices[2], color, normals[4]),

        // LEFT
        Vertex(vertices[0], color, normals[3]),
        Vertex(vertices[1], color, normals[3]),
        Vertex(vertices[7], color, normals[3]),
        Vertex(vertices[2], color, normals[3]),
        Vertex(vertices[1], color, normals[3]),
        Vertex(vertices[7], color, normals[3]),

        // RIGHT
        Vertex(vertices[6], color, normals[2]),
        Vertex(vertices[5], color, normals[2]),
        Vertex(vertices[4], color, normals[2]),
        Vertex(vertices[6], color, normals[2]),
        Vertex(vertices[3], color, normals[2]),
        Vertex(vertices[4], color, normals[2]),

        // TOP
        Vertex(vertices[2], color, normals[0]),
        Vertex(vertices[1], color, normals[0]),
        Vertex(vertices[6], color, normals[0]),
        Vertex(vertices[2], color, normals[0]),
        Vertex(vertices[3], color, normals[0]),
        Vertex(vertices[6], color, normals[0]),

        // BOTTOM
        Vertex(vertices[0], color, normals[1]),
        Vertex(vertices[4], color, normals[1]),
        Vertex(vertices[5], color, normals[1]),
        Vertex(vertices[0], color, normals[1]),
        Vertex(vertices[4], color, normals[1]),
        Vertex(vertices[7], color, normals[1]),
    };

    cube.mNumberOfVertices = sizeof(vertexData) / sizeof(*vertexData);

    cube.mVertices = new Vertex[cube.mNumberOfVertices];
    memcpy(cube.mVertices, vertexData, sizeof(vertexData));

    cube.mScale = scale;
    cube.mRotate = rotate;
    cube.mTranslate = translate;

    return cube;
}

Shape ShapeGenerator::GenerateLine(glm::vec3 first, glm::vec3 second, glm::vec3 color)
{
    Shape line;

    Vertex vertices[] = {
        Vertex(first, color, glm::vec3(0.0f)),
        Vertex(second, color, glm::vec3(0.0f))
    };

    line.mNumberOfVertices = sizeof(vertices) / sizeof(*vertices);

    line.mVertices = new Vertex[line.mNumberOfVertices];
    memcpy(line.mVertices, vertices, sizeof(vertices));

    return line;
}

Shape ShapeGenerator::GenerateTeddy(glm::vec3 color, glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate)
{
    Shape teddy;

    vector<Vertex> vertices;
    loadOBJ("teddy.obj", vertices, color);

    teddy.mNumberOfVertices = vertices.size();

    teddy.mVertices = new Vertex[teddy.mNumberOfVertices];
    memcpy(teddy.mVertices, &vertices[0], sizeof(Vertex) * vertices.size());

    teddy.mScale = scale;
    teddy.mRotate = rotate;
    teddy.mTranslate = translate;

    return teddy;
}

Shape ShapeGenerator::GeneratePoint(glm::vec2 position, glm::vec3 color)
{
    Shape point;

    Vertex vertices[] = {
        Vertex(glm::vec3(position, 0.0f), color, glm::vec3(0.0f))
    };

    point.mNumberOfVertices = sizeof(vertices) / sizeof(*vertices);

    point.mVertices = new Vertex[point.mNumberOfVertices];
    memcpy(point.mVertices, vertices, sizeof(vertices));

    return point;
}

Shape ShapeGenerator::GenerateTrack(glm::vec3 color, glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate)
{
    Shape track;

    glm::vec3 vertices[] = {
        glm::vec3(-2.0f, -0.1f, -0.1f), // 0
        glm::vec3(-2.0f, +0.1f, -0.1f), // 1
        glm::vec3(-2.0f, +0.1f, +0.1f), // 2
        glm::vec3(+2.0f, +0.1f, +0.1f), // 3
        glm::vec3(+2.0f, -0.1f, +0.1f), // 4
        glm::vec3(+2.0f, -0.1f, -0.1f), // 5
        glm::vec3(+2.0f, +0.1f, -0.1f), // 6
        glm::vec3(-2.0f, -0.1f, +0.1f), // 7
    };

    glm::vec3 normals[] = {
        glm::vec3(0.0f, +1.0f, 0.0f), // 0
        glm::vec3(0.0f, -1.0f, 0.0f), // 1
        glm::vec3(+1.0f, 0.0f, 0.0f), // 2
        glm::vec3(-1.0f, 0.0f, 0.0f), // 3
        glm::vec3(0.0f, 0.0f, +1.0f), // 4
        glm::vec3(0.0f, 0.0f, -1.0f)  // 5
    };

    glm::vec2 uvs[] = {
        glm::vec2(0.0f, 0.0f), // 0
        glm::vec2(0.0f, 1.0f), // 1
        glm::vec2(1.0f, 0.0f), // 2
        glm::vec2(1.0f, 1.0f)  // 3
    };

    Vertex vertexData[] = {
        // FRONT
        Vertex(vertices[0], color, normals[5], uvs[0]),
        Vertex(vertices[6], color, normals[5], uvs[3]),
        Vertex(vertices[1], color, normals[5], uvs[2]),
        Vertex(vertices[0], color, normals[5], uvs[0]),
        Vertex(vertices[6], color, normals[5], uvs[3]),
        Vertex(vertices[5], color, normals[5], uvs[1]),

        // BACK
        Vertex(vertices[7], color, normals[4], uvs[0]),
        Vertex(vertices[4], color, normals[4], uvs[1]),
        Vertex(vertices[2], color, normals[4], uvs[2]),
        Vertex(vertices[3], color, normals[4], uvs[3]),
        Vertex(vertices[4], color, normals[4], uvs[1]),
        Vertex(vertices[2], color, normals[4], uvs[2]),

        // LEFT
        Vertex(vertices[0], color, normals[3], uvs[1]),
        Vertex(vertices[1], color, normals[3], uvs[3]),
        Vertex(vertices[7], color, normals[3], uvs[0]),
        Vertex(vertices[2], color, normals[3], uvs[2]),
        Vertex(vertices[1], color, normals[3], uvs[3]),
        Vertex(vertices[7], color, normals[3], uvs[0]),

        // RIGHT
        Vertex(vertices[6], color, normals[2], uvs[2]),
        Vertex(vertices[5], color, normals[2], uvs[0]),
        Vertex(vertices[4], color, normals[2], uvs[1]),
        Vertex(vertices[6], color, normals[2], uvs[2]),
        Vertex(vertices[3], color, normals[2], uvs[3]),
        Vertex(vertices[4], color, normals[2], uvs[1]),

        // TOP
        Vertex(vertices[2], color, normals[0], uvs[2]),
        Vertex(vertices[1], color, normals[0], uvs[0]),
        Vertex(vertices[6], color, normals[0], uvs[1]),
        Vertex(vertices[2], color, normals[0], uvs[2]),
        Vertex(vertices[3], color, normals[0], uvs[3]),
        Vertex(vertices[6], color, normals[0], uvs[1]),

        // BOTTOM
        Vertex(vertices[0], color, normals[1], uvs[2]),
        Vertex(vertices[4], color, normals[1], uvs[1]),
        Vertex(vertices[5], color, normals[1], uvs[3]),
        Vertex(vertices[0], color, normals[1], uvs[2]),
        Vertex(vertices[4], color, normals[1], uvs[1]),
        Vertex(vertices[7], color, normals[1], uvs[0]),
    };

    track.mNumberOfVertices = sizeof(vertexData) / sizeof(*vertexData);

    track.mVertices = new Vertex[track.mNumberOfVertices];
    memcpy(track.mVertices, vertexData, sizeof(vertexData));

    track.mScale = scale;
    track.mRotate = rotate;
    track.mTranslate = translate;

    return track;
}

Shape ShapeGenerator::GenerateCatmullRomSpline(vector<glm::vec3>& controlPoints, float index)
{
    Shape spline;
    vector<Vertex> vertices;

    for (float t = 0.0f; t < 1.0f * controlPoints.size(); t++)
    {
        int i0 = glm::clamp<int>(t - 1, 0, controlPoints.size() - 1);
        int i1 = glm::clamp<int>(t, 0, controlPoints.size() - 1);
        int i2 = glm::clamp<int>(t + 1, 0, controlPoints.size() - 1);
        int i3 = glm::clamp<int>(t + 2, 0, controlPoints.size() - 1);

        float local = glm::fract(t);
        float nextLocal = glm::fract(t) + 1.0f;

        Subdivide(local, nextLocal, controlPoints[i0], controlPoints[i1], controlPoints[i2], controlPoints[i3], vertices, index, OpenGLWindow::mCurvatureToggle);
    }

    spline.mNumberOfVertices = vertices.size();

    spline.mVertices = new Vertex[spline.mNumberOfVertices];
    memcpy(spline.mVertices, &vertices[0], sizeof(Vertex) * vertices.size());

    return spline;
}

Shape ShapeGenerator::GenerateTerrain(glm::vec3 color)
{
    Shape terrain;

    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

    Vertex vertices[] = {
        Vertex(glm::vec3(0.0f,0.0f,0.0f), color, normal, glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f,0.0f,0.5f), color, normal, glm::vec2(0.0f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),

        Vertex(glm::vec3(0.0f,0.0f,0.0f), color, normal, glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,0.0f), color, normal, glm::vec2(0.5f, 0.0f)),

        Vertex(glm::vec3(0.5f,0.0f,0.0f), color, normal, glm::vec2(0.5f, 0.0f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),
        Vertex(glm::vec3(1.0f,0.0f,0.5f), color, normal, glm::vec2(1.0f, 0.5f)),

        Vertex(glm::vec3(0.5f,0.0f,0.0f), color, normal, glm::vec2(0.5f, 0.0f)),
        Vertex(glm::vec3(1.0f,0.0f,0.5f), color, normal, glm::vec2(1.0f, 0.5f)),
        Vertex(glm::vec3(1.0f,0.0f,0.0f), color, normal, glm::vec2(1.0f, 0.0f)),

        Vertex(glm::vec3(0.0f,0.0f,0.5f), color, normal, glm::vec2(0.0f, 0.5f)),
        Vertex(glm::vec3(0.0f,0.0f,1.0f), color, normal, glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(0.5f,0.0f,1.0f), color, normal, glm::vec2(0.5f, 1.0f)),

        Vertex(glm::vec3(0.0f,0.0f,0.5f), color, normal, glm::vec2(0.0f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,1.0f), color, normal, glm::vec2(0.5f, 1.0f)),

        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,1.0f), color, normal, glm::vec2(0.5f, 1.0f)),
        Vertex(glm::vec3(1.0f,0.0f,1.0f), color, normal, glm::vec2(1.0f, 1.0f)),

        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),
        Vertex(glm::vec3(1.0f,0.0f,1.0f), color, normal, glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(1.0f,0.0f,0.5f), color, normal, glm::vec2(1.0f, 0.5f))
    };

    terrain.mNumberOfVertices = sizeof(vertices) / sizeof(*vertices);

    terrain.mVertices = new Vertex[terrain.mNumberOfVertices];
    memcpy(terrain.mVertices, vertices, sizeof(vertices));

    terrain.mScale = glm::vec3(10.0f);
    return terrain;
}