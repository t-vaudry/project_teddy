#include "stdafx.h"
#include "ShapeGenerator.h"

#include <iostream>

float ShapeGenerator::mWallWidth = 1.0f;
float ShapeGenerator::mWallHeight = 1.0f;
float ShapeGenerator::mWallDepth = 1.0f;

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

    glm::vec2 uvs[] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f)
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

    cube.mNumberOfVertices = sizeof(vertexData) / sizeof(*vertexData);

    cube.mVertices = new Vertex[cube.mNumberOfVertices];
    memcpy(cube.mVertices, vertexData, sizeof(vertexData));

    cube.mScale = scale;
    cube.mRotate = rotate;
    cube.mTranslate = translate;

    cube.InitializeBoundingSphere();

    //DEBUG:
    std::cout << cube.mCenter[0] << ", " << cube.mCenter[1] << ", " << cube.mCenter[2] << endl;
    std::cout << cube.mRadius << endl;
    return cube;
}

Shape ShapeGenerator::GenerateQuad(glm::vec3 color, float width, float height, float depth, glm::vec3 rotate, glm::vec3 translate)
{
    Shape quad;

    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;

    glm::vec3 vertices[] = {
        glm::vec3(-w, -h, -d), // 0
        glm::vec3(-w, h, -d), // 1
        glm::vec3(-w, h, d), // 2
        glm::vec3(w, h, d), // 3
        glm::vec3(w, -h, d), // 4
        glm::vec3(w, -h, -d), // 5
        glm::vec3(w, h, -d), // 6
        glm::vec3(-w, -h, d), // 7
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
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f)
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

    quad.mNumberOfVertices = sizeof(vertexData) / sizeof(*vertexData);

    quad.mVertices = new Vertex[quad.mNumberOfVertices];
    memcpy(quad.mVertices, vertexData, sizeof(vertexData));

    quad.mScale = glm::vec3(1.0f);
    quad.mRotate = rotate;
    quad.mTranslate = translate;

    quad.InitializeBoundingSphere();

    return quad;
}

Shape ShapeGenerator::GenerateWall()
{
    return GenerateQuad(glm::vec3(1.0f), mWallWidth, mWallHeight, mWallDepth);
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

    //line.InitializeBoundingSphere();

    return line;
}

Shape ShapeGenerator::GenerateOBJ(const char* path, glm::vec3 color, glm::vec3 scale, glm::vec3 rotate, glm::vec3 translate)
{
    Shape obj;

    vector<Vertex> vertices;
    loadOBJ(path, vertices, color);

    obj.mNumberOfVertices = vertices.size();

    obj.mVertices = new Vertex[obj.mNumberOfVertices];
    memcpy(obj.mVertices, &vertices[0], sizeof(Vertex) * vertices.size());

    obj.mScale = scale;
    obj.mRotate = rotate;
    obj.mTranslate = translate;

    obj.InitializeBoundingSphere();

    //DEBUG:
    std::cout << "obj: " << endl;
    std::cout << obj.mCenter[0] << ", " << obj.mCenter[1] << ", " << obj.mCenter[2] << endl;
    std::cout << obj.mRadius << endl;

    return obj;
}

Shape ShapeGenerator::GeneratePoint(glm::vec2 position, glm::vec3 color)
{
    Shape point;

    Vertex vertices[] = {
        Vertex(glm::vec3(position.x, -1.0f, position.y), color, glm::vec3(0.0f))
    };

    point.mNumberOfVertices = sizeof(vertices) / sizeof(*vertices);

    point.mVertices = new Vertex[point.mNumberOfVertices];
    memcpy(point.mVertices, vertices, sizeof(vertices));

    return point;
}

Shape ShapeGenerator::GenerateTerrain(glm::vec3 color, float height, bool inverse)
{
    Shape terrain;

    glm::vec3 normal;

    if (inverse)
        normal = glm::vec3(0.0f, -1.0f, 0.0f);
    else
        normal = glm::vec3(0.0f, 1.0f, 0.0f);

    Vertex vertices[] = {
        Vertex(glm::vec3(0.0f,0.0f,0.0f), color, normal, glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f,0.0f,0.5f), color, normal, glm::vec2(0.0f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),

        Vertex(glm::vec3(0.5f,0.0f,0.0f), color, normal, glm::vec2(0.5f, 0.0f)),
        Vertex(glm::vec3(0.0f,0.0f,0.0f), color, normal, glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),

        Vertex(glm::vec3(0.5f,0.0f,0.0f), color, normal, glm::vec2(0.5f, 0.0f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),
        Vertex(glm::vec3(1.0f,0.0f,0.5f), color, normal, glm::vec2(1.0f, 0.5f)),

        Vertex(glm::vec3(1.0f,0.0f,0.5f), color, normal, glm::vec2(1.0f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,0.0f), color, normal, glm::vec2(0.5f, 0.0f)),
        Vertex(glm::vec3(1.0f,0.0f,0.0f), color, normal, glm::vec2(1.0f, 0.0f)),

        Vertex(glm::vec3(1.0f,0.0f,0.5f), color, normal, glm::vec2(1.0f, 0.5f)),
        Vertex(glm::vec3(1.0f,0.0f,1.0f), color, normal, glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),

        Vertex(glm::vec3(1.0f,0.0f,1.0f), color, normal, glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(0.5f,0.0f,1.0f), color, normal, glm::vec2(0.5f, 1.0f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),

        Vertex(glm::vec3(0.0f,0.0f,0.5f), color, normal, glm::vec2(0.0f, 0.5f)),
        Vertex(glm::vec3(0.0f,0.0f,1.0f), color, normal, glm::vec2(0.0f, 1.0f)),
        Vertex(glm::vec3(0.5f,0.0f,1.0f), color, normal, glm::vec2(0.5f, 1.0f)),

        Vertex(glm::vec3(0.0f,0.0f,0.5f), color, normal, glm::vec2(0.0f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,0.5f), color, normal, glm::vec2(0.5f, 0.5f)),
        Vertex(glm::vec3(0.5f,0.0f,1.0f), color, normal, glm::vec2(0.5f, 1.0f)),
    };

    terrain.mNumberOfVertices = sizeof(vertices) / sizeof(*vertices);

    terrain.mVertices = new Vertex[terrain.mNumberOfVertices];
    memcpy(terrain.mVertices, vertices, sizeof(vertices));

    terrain.mScale = glm::vec3(1.0f);
    terrain.mTranslate = glm::vec3(24.5f, height, 25.0f);
    return terrain;
}

void ShapeGenerator::SetWallDimensions(float w, float h, float d)
{
    mWallWidth = w;
    mWallHeight = h;
    mWallDepth = d;
}

vector<glm::mat4> ShapeGenerator::GenerateWallModelMatrices()
{
    vector<glm::mat4> model_matrices;

    glm::mat4 model_matrix = glm::mat4(1.0f);
    int backWallLength = 10;
    int sideWallLength = 10;

    //S1
    model_matrices.push_back(glm::translate(model_matrix, glm::vec3(25.0f, 0.0f, 25.0f)));
    for (int i = 0; i < backWallLength; i++)
    {
        model_matrices.push_back(glm::translate(model_matrices[i], glm::vec3(mWallWidth, 0.0f, 0.0f)));
    }

    glm::mat4 nextWallBegin = model_matrices[model_matrices.size() - 1];

    nextWallBegin = glm::translate(nextWallBegin, glm::vec3(+mWallWidth / 2.0f, 0.0f, 0.0f));
    nextWallBegin = glm::rotate(nextWallBegin, 90.0f * 0.0174533f, glm::vec3(0.0f, 1.0f, 0.0f));
    nextWallBegin = glm::translate(nextWallBegin, glm::vec3(-mWallWidth / 2.0f, 0.0f, 0.0f));

    //S6
    for (int i = 0; i < sideWallLength; i++)
    {
        model_matrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, 0.0f, -(backWallLength + 1))));
    }

    //S3
    for (int i = 0; i < backWallLength + 1; i++)
    {
        //Doorway
        if (i == backWallLength / 4)
            continue;

        model_matrices.push_back(glm::translate(model_matrices[i], glm::vec3(0.0f, 0.0f, mWallWidth * sideWallLength/2.0f)));
    }

    //S5
    for (int i = 0; i < backWallLength/2 + 1; i++)
    {
        model_matrices.push_back(glm::translate(model_matrices[i], glm::vec3(0.0f, 0.0f, mWallWidth * sideWallLength)));
    }

    //S4
    for (int i = 0; i < sideWallLength; i++)
    {
        //Doorway
        if (i == sideWallLength / 4)
            continue;

        model_matrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, 0.0f, -(backWallLength / 2))));
    }

    //S2
    for (int i = 0; i < sideWallLength/2 + 1; i++)
    {
        model_matrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, 0.0f, 0.0f)));
    }

    return model_matrices;
}

vector<glm::mat4> ShapeGenerator::GeneratePlaneModelMatrices(Shape* shape)
{
    int index = 0;
    vector<glm::mat4> modelMatrices;
    for (float i = 0.0f; i < 11.0f; i += 1.0f)
    {
        for (float j = 0.0f; j < 10.0f; j += 1.0f)
        {
            if (i < 6.0f || j < 5.0f)
            {
                modelMatrices.push_back(glm::mat4(1.0f));
                modelMatrices[index] = glm::translate(modelMatrices[index], glm::vec3(i, 0.0f, j) + shape->mTranslate);
                modelMatrices[index] = glm::scale(modelMatrices[index], shape->mScale);
                index++;
            }
        }
    }

    return modelMatrices;
}