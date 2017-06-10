#pragma once
#include <vector>
#include <Shape.h>
#include <objloader.h>
#include <OpenGLWindow.h>

using namespace std;

class ShapeGenerator
{
public:
    static Shape GenerateCube(glm::vec3 color, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f), glm::vec3 translate = glm::vec3(0.0f));
    static Shape GenerateQuad(glm::vec3 color, float width, float height, float depth, glm::vec3 rotate = glm::vec3(0.0f), glm::vec3 translate = glm::vec3(0.0f));
    static Shape GenerateLine(glm::vec3 first, glm::vec3 second, glm::vec3 color);
    static Shape GenerateOBJ(const char* path, glm::vec3 color, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f), glm::vec3 translate = glm::vec3(0.0f));
    static Shape GeneratePoint(glm::vec2 position, glm::vec3 color);
    static Shape GenerateTerrain(glm::vec3 color);
};

