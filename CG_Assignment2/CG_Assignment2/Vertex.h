#pragma once
#include <glm.hpp>

struct Vertex
{
    Vertex() {}
    Vertex(glm::vec3 position, glm::vec3 color, glm::vec3 normal)
    {
        mPosition = position;
        mColor = color;
        mSurfaceNormal = normal;
    }

    glm::vec3 mPosition;
    glm::vec3 mColor;
    glm::vec3 mSurfaceNormal;
};
