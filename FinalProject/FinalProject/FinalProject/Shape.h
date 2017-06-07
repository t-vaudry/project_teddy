#pragma once
#include <glew.h>
#include "Vertex.h"
#include "Miniball.hpp"

struct Shape
{
    Shape()
        : mVertices(NULL)
        , mNumberOfVertices(0)
        , mScale(glm::vec3(1.0f))
        , mRotate(glm::vec3(0.0f))
        , mTranslate(glm::vec3(0.0f))
        , mCenter(glm::vec3(0.0f))
        , mRadius(1.0f)
    {}

    Vertex* mVertices;
    GLuint mNumberOfVertices;
    glm::vec3 mScale;
    glm::vec3 mRotate;
    glm::vec3 mTranslate;
    
    //Bounding sphere
    glm::vec3 mCenter;
    float mRadius;

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

    void InitializeBoundingSphere()
    {
        // Store each vertex position (vec3) in an array of points
        // ----------------------------------------------------
        float** points = new float*[mNumberOfVertices];
        for (GLuint i = 0; i < mNumberOfVertices; ++i)
        {
            float* p = new float[3]; //3 dimensions
            for (int j = 0; j < 3; ++j)
            {
                p[j] = mVertices[i].mPosition[j];
            }
            points[i] = p;
        }

        // define the types of iterators through the points and their coordinates
        // ----------------------------------------------------------------------
        typedef float* const* PointIterator;
        typedef const float* CoordIterator;

        // create an instance of Miniball
        // ------------------------------
        typedef Miniball::
            Miniball <Miniball::CoordAccessor<PointIterator, CoordIterator> >
            BoundingSphere;
        BoundingSphere boundingSphere(3, points, points + mNumberOfVertices);

        //Set the shape's center and radius
        const float* centerCoords = boundingSphere.center();
        mCenter[0] = centerCoords[0];
        mCenter[1] = centerCoords[1];
        mCenter[2] = centerCoords[2];
        mRadius = sqrt(2.0f/3.0f) * sqrt(glm::length(boundingSphere.squared_radius()));

        //Translate and scale the centre and radius according to the shape's translations
        mCenter *= mScale[0];
        mCenter += mTranslate;
        mRadius *= mScale[0];
    }

    float IsSelected(glm::vec3& world_ray, glm::vec3& camera_position)
    {
        glm::vec3 difference = camera_position - mCenter;
        float b = glm::dot(world_ray, difference);
        float c = glm::dot(difference, difference) - pow(mRadius, 2);

        float square = pow(b, 2) - c;

        if (square < 0)
            return -1.0f;
        else if (square == 0)
            return -b;
        else
        {
            float value = sqrt(square);
            float t1 = -b + value;
            float t2 = -b - value;

            return t1 < t2 ? t1 : t2;
        }
    }
};
