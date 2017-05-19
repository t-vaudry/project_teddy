#pragma once
#include <glm.hpp>

struct Vertex
{
    Vertex() {}
    Vertex(glm::vec3 position, glm::vec3 color)
    {
        mPosition = position;
        mColor = color;
    }

    glm::vec3 mPosition;
    glm::vec3 mColor;
};
