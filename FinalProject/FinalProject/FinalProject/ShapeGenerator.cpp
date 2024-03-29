#include <stdafx.h>
#include <ShapeGenerator.h>

#include <iostream>

float ShapeGenerator::mWallWidth = 1.0f;
float ShapeGenerator::mWallHeight = 1.0f;
float ShapeGenerator::mWallDepth = 1.0f;
vector<glm::mat4> ShapeGenerator::mTopWindowMatrices;
vector<glm::mat4> ShapeGenerator::mBottomWindowMatrices;
vector<glm::mat4> ShapeGenerator::mWindowMatrices;

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

Shape ShapeGenerator::GenerateAABB(vector<glm::vec3>& points)
{
    Shape aabb;

    float h = 1.0f;
    glm::vec3 vertices[] = {
        glm::vec3(points[0].x, -h, points[0].z), // 0
        glm::vec3(points[0].x, h, points[0].z), // 1
        glm::vec3(points[1].x, h, points[1].z), // 2
        glm::vec3(points[3].x, h, points[3].z), // 3
        glm::vec3(points[3].x, -h, points[3].z), // 4
        glm::vec3(points[2].x, -h, points[2].z), // 5
        glm::vec3(points[2].x, h, points[2].z), // 6
        glm::vec3(points[1].x, -h, points[1].z), // 7
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

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

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

    aabb.mNumberOfVertices = sizeof(vertexData) / sizeof(*vertexData);

    aabb.mVertices = new Vertex[aabb.mNumberOfVertices];
    memcpy(aabb.mVertices, vertexData, sizeof(vertexData));

    aabb.mScale = glm::vec3(1.0f);
    aabb.mRotate = glm::vec3(0.0f);
    aabb.mTranslate = glm::vec3(0.0f);

    return aabb;
}

Shape ShapeGenerator::GenerateWall()
{
    return GenerateQuad(glm::vec3(1.0f), mWallWidth, mWallHeight, mWallDepth);
}

Shape ShapeGenerator::GenerateWindowWall()
{
    return GenerateQuad(glm::vec3(1.0f), mWallWidth, mWallHeight/3.0f, mWallDepth);
}

Shape ShapeGenerator::GenerateWindow()
{
    Shape shape = GenerateQuad(glm::vec3(0.0f, 0.0f, 1.0f), mWallWidth, mWallHeight, 0.00075f);
    shape.mAlpha = 0.05f;
    return shape;
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
    obj.InitializeBoundingBox();

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

Shape ShapeGenerator::GenerateCrosshair()
{
    Shape crosshair;

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    Vertex vertices[] = {
        Vertex(glm::vec3(0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(20.0f, 0.0f, 0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-20.0f, 0.0f, 0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 20.0f, 0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, -20.0f, 0.0f), color, glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)),
    };

    crosshair.mNumberOfVertices = sizeof(vertices) / sizeof(*vertices);

    crosshair.mVertices = new Vertex[crosshair.mNumberOfVertices];
    memcpy(crosshair.mVertices, vertices, sizeof(vertices));

    return crosshair;
}

Shape ShapeGenerator::GenerateMenu()
{
    Shape menu;

    glm::vec3 color = glm::vec3(1.0f);

    glm::vec2 uvs[] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f)
    };

    Vertex vertices[] = {
        Vertex(glm::vec3(-0.375 * OpenGLWindow::mWidth, -0.25 * OpenGLWindow::mHeight, 0.0f), color, glm::vec3(0.0f, 1.0f, 0.0f), uvs[0]),
        Vertex(glm::vec3(0.375 * OpenGLWindow::mWidth, -0.25 * OpenGLWindow::mHeight, 0.0f), color, glm::vec3(0.0f, 1.0f, 0.0f), uvs[2]),
        Vertex(glm::vec3(-0.375 * OpenGLWindow::mWidth, 0.25 * OpenGLWindow::mHeight, 0.0f), color, glm::vec3(0.0f, 1.0f, 0.0f), uvs[1]),
        Vertex(glm::vec3(0.375 * OpenGLWindow::mWidth, -0.25 * OpenGLWindow::mHeight, 0.0f), color, glm::vec3(0.0f, 1.0f, 0.0f), uvs[2]),
        Vertex(glm::vec3(0.375 * OpenGLWindow::mWidth, 0.25 * OpenGLWindow::mHeight, 0.0f), color, glm::vec3(0.0f, 1.0f, 0.0f), uvs[3]),
        Vertex(glm::vec3(-0.375 * OpenGLWindow::mWidth, 0.25 * OpenGLWindow::mHeight, 0.0f), color, glm::vec3(0.0f, 1.0f, 0.0f), uvs[1]),
    };

    menu.mNumberOfVertices = sizeof(vertices) / sizeof(*vertices);

    menu.mVertices = new Vertex[menu.mNumberOfVertices];
    memcpy(menu.mVertices, vertices, sizeof(vertices));

    return menu;
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
        if (i == sideWallLength / 4 || i == sideWallLength / 4 + 1)
        {
            mTopWindowMatrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, mWallHeight / 2.0f, -(backWallLength + 1))));
            mBottomWindowMatrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, -mWallHeight / 2.0f, -(backWallLength + 1))));
            mWindowMatrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, 0.0f, -(backWallLength + 1))));
            continue;
        }
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
        //Window
        if (i == backWallLength / 4 || i == backWallLength / 4 + 1)
        {
            mTopWindowMatrices.push_back(glm::translate(model_matrices[i], glm::vec3(0.0f, mWallHeight / 2.0f, mWallWidth * sideWallLength)));
            mBottomWindowMatrices.push_back(glm::translate(model_matrices[i], glm::vec3(0.0f, -mWallHeight / 2.0f, mWallWidth * sideWallLength)));
            mWindowMatrices.push_back(glm::translate(model_matrices[i], glm::vec3(0.0f, 0.0f, mWallWidth * sideWallLength)));
            continue;
        }

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
        if (i == sideWallLength / 4 - 1 || i == sideWallLength / 4 || i == sideWallLength / 4 + 1)
        {
            mTopWindowMatrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, mWallHeight/2.0f, 0.0f)));
            mBottomWindowMatrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, -mWallHeight / 2.0f, 0.0f)));
            mWindowMatrices.push_back(glm::translate(nextWallBegin, glm::vec3(-mWallWidth * i, 0.0f, 0.0f)));
            continue;
        }
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

vector<glm::mat4> ShapeGenerator::GetTopWindowMatrices()
{
    return mTopWindowMatrices;
}

vector<glm::mat4> ShapeGenerator::GetBottomWindowMatrices()
{
    return mBottomWindowMatrices;
}

vector<glm::mat4> ShapeGenerator::GetWindowMatrices()
{
    return mWindowMatrices;
}