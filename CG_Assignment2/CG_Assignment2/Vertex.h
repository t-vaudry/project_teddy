#pragma once
#include <glm.hpp>

struct Vertex
{
    Vertex() {}
    Vertex(glm::vec3 position, glm::vec3 color, glm::vec3 normal, glm::vec2 uv = glm::vec2(0.0f)) //TODO: remove predec
    {
        mPosition = position;
        mColor = color;
        mSurfaceNormal = normal;
        mUV = uv;
    }

    glm::vec3 mPosition;
    glm::vec3 mColor;
    glm::vec3 mSurfaceNormal;
    glm::vec2 mUV;
};
