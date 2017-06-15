#pragma once
#include <vector>
#include <glew.h>
#include <gtx/transform.hpp>
#include "Vertex.h"
#include "Miniball.hpp"

using namespace std;

struct BoundingBox
{
    BoundingBox(glm::vec3 max, glm::vec3 min)
        : mMax(max)
        , mMin(min)
    {
        mScale = glm::vec3(1.0f);
        mRotate = glm::vec3(0.0f);
        mTranslate = glm::vec3(0.0f);

        mPoints.push_back(glm::vec3(mMin.x, 0.0f, mMin.z));
        mPoints.push_back(glm::vec3(mMin.x, 0.0f, mMax.z));
        mPoints.push_back(glm::vec3(mMax.x, 0.0f, mMin.z));
        mPoints.push_back(glm::vec3(mMax.x, 0.0f, mMax.z));
    }

    glm::vec3 mMax;
    glm::vec3 mMin;

    vector<glm::vec3> mPoints;

    glm::vec3 mScale;
    glm::vec3 mRotate;
    glm::vec3 mTranslate;

    void Set()
    {
        vector<glm::vec3> newPoints;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, mTranslate);
        model = glm::rotate(model, glm::radians(mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(mRotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(mRotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, mScale);

        float maxX = -FLT_MAX;
        float minX = FLT_MAX;
        float maxZ = -FLT_MAX;
        float minZ = FLT_MAX;

        for (unsigned int i = 0; i < mPoints.size(); i++)
        {
            newPoints.push_back(glm::vec3(model * glm::vec4(mPoints[i], 1.0f)));

            if (newPoints[i].x > maxX)
                maxX = newPoints[i].x;
            if (newPoints[i].x < minX)
                minX = newPoints[i].x;

            if (newPoints[i].z > maxZ)
                maxZ = newPoints[i].z;
            if (newPoints[i].z < minZ)
                minZ = newPoints[i].z;
        }

        mMax = glm::vec3(maxX, 0.0f, maxZ);
        mMin = glm::vec3(minX, 0.0f, minZ);
    }

    bool Intersect(BoundingBox& box)
    {
        return (mMin.x <= box.mMax.x && mMax.x >= box.mMin.x) &&
               (mMin.z <= box.mMax.z && mMax.z >= box.mMin.z);
    }
};

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
        , mBox(BoundingBox(glm::vec3(0.0f), glm::vec3(0.0f)))
        , mAlpha(1.0f)
        , mValidPos(true)
    {}

    // Vertex information
    Vertex* mVertices;
    GLuint mNumberOfVertices;

    // Transformations
    glm::vec3 mScale;
    glm::vec3 mRotate;
    glm::vec3 mTranslate;
    
    // Bounding sphere
    glm::vec3 mCenter;
    float mRadius;

    // Bounding box
    BoundingBox mBox;

    // Transparency
    float mAlpha;

    //Positioning
    bool mValidPos;

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
        mCenter.x = centerCoords[0];
        mCenter.y = centerCoords[1];
        mCenter.z = centerCoords[2];
        //mRadius = sqrt(2.0f/3.0f) * sqrt(glm::length(boundingSphere.squared_radius()));
        mRadius = sqrt(glm::length(boundingSphere.squared_radius())); //Seems to work better with more compelx shapes than sqrt(2/3)

        //Translate and scale the centre and radius according to the shape's translations
        mCenter *= mScale[0];
        mCenter += mTranslate;
        mRadius *= (mScale[0] + mScale[1] + mScale[2])/3.0f;
    }

    void InitializeBoundingBox()
    {
        glm::vec3 max = mVertices[0].mPosition;
        glm::vec3 min = mVertices[0].mPosition;

        for (unsigned int i = 1; i < mNumberOfVertices; i++)
        {
            if (mVertices[i].mPosition.x < min.x)
            {
                min.x = mVertices[i].mPosition.x;
            }
            else if (mVertices[i].mPosition.x > max.x)
            {
                max.x = mVertices[i].mPosition.x;
            }

            if (mVertices[i].mPosition.y < min.y)
            {
                min.y = mVertices[i].mPosition.y;
            }
            else if (mVertices[i].mPosition.y > max.y)
            {
                max.y = mVertices[i].mPosition.y;
            }

            if (mVertices[i].mPosition.z < min.z)
            {
                min.z = mVertices[i].mPosition.z;
            }
            else if (mVertices[i].mPosition.z > max.z)
            {
                max.z = mVertices[i].mPosition.z;
            }
        }

        mBox = BoundingBox(max, min);
        mBox.mScale = mScale;
        mBox.mRotate = mRotate;
        mBox.mTranslate = mTranslate;

        mBox.Set();
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
