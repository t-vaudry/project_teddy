#pragma once
#include <glew.h>
#include "Vertex.h"

struct Shape
{
    Shape()
        : mVertices(NULL)
        , mNumberOfVertices(0)
        , mScale(glm::vec3(1.0f))
        , mRotate(glm::vec3(0.0f))
        , mTranslate(glm::vec3(0.0f)) {}

    Vertex* mVertices;
    GLuint mNumberOfVertices;
    glm::vec3 mScale;
    glm::vec3 mRotate;
    glm::vec3 mTranslate;

    int GetVertexBufferSize()
    {
        return mNumberOfVertices * sizeof(Vertex);
    }

    void CleanUp()
    {
        if (mVertices)
        {
            delete[] mVertices;
            mVertices = NULL;
        }

        mNumberOfVertices = 0;
    }
};
