#include "stdafx.h"
#include "ShapeGenerator.h"

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

    Vertex vertexData[] = {
        // FRONT
        Vertex(vertices[0], color),
        Vertex(vertices[6], color),
        Vertex(vertices[1], color),
        Vertex(vertices[0], color),
        Vertex(vertices[6], color),
        Vertex(vertices[5], color),

        // BACK
        Vertex(vertices[7], color),
        Vertex(vertices[4], color),
        Vertex(vertices[2], color),
        Vertex(vertices[3], color),
        Vertex(vertices[4], color),
        Vertex(vertices[2], color),

        // LEFT
        Vertex(vertices[0], color),
        Vertex(vertices[1], color),
        Vertex(vertices[7], color),
        Vertex(vertices[2], color),
        Vertex(vertices[1], color),
        Vertex(vertices[7], color),

        // RIGHT
        Vertex(vertices[6], color),
        Vertex(vertices[5], color),
        Vertex(vertices[4], color),
        Vertex(vertices[6], color),
        Vertex(vertices[3], color),
        Vertex(vertices[4], color),

        // TOP
        Vertex(vertices[2], color),
        Vertex(vertices[1], color),
        Vertex(vertices[6], color),
        Vertex(vertices[2], color),
        Vertex(vertices[3], color),
        Vertex(vertices[6], color),

        // BOTTOM
        Vertex(vertices[0], color),
        Vertex(vertices[4], color),
        Vertex(vertices[5], color),
        Vertex(vertices[0], color),
        Vertex(vertices[4], color),
        Vertex(vertices[7], color),
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
        Vertex(first, color),
        Vertex(second, color)
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