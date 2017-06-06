#pragma once
#include <glm.hpp>

glm::vec3 CatmullRom(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, float t)
{
    float s = 0.5f;

    // Create basis matrix
    glm::mat4x4 basis = glm::mat4x4(glm::vec4(-s, 2-s, s-2, s),
                                glm::vec4(2*s, s-3, 3-2*s, -s),
                                glm::vec4(-s, 0, s, 0),
                                glm::vec4(0, 1, 0, 0));

    // Create control matrix
    glm::mat4x3 control = glm::mat4x3(p1, p2, p3, p4);

    // Create local vector
    glm::vec4 local = glm::vec4(t*t*t, t*t, t, 1);

    return control * basis * local;
}

void Subdivide(float t, float nextT, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, vector<Vertex>& vertices, float index, bool curvatureBased)
{
    float mid = (t + nextT) / 2.0f;

    // Determine vertices for current, mid and next point
    Vertex vertex = Vertex(CatmullRom(p1, p2, p3, p4, t), glm::vec3(1.0f), glm::vec3(0.0f));
    Vertex midVertex = Vertex(CatmullRom(p1, p2, p3, p4, mid), glm::vec3(1.0f), glm::vec3(0.0f));
    Vertex nextVertex = Vertex(CatmullRom(p1, p2, p3, p4, nextT), glm::vec3(1.0f), glm::vec3(0.0f));

    // Calculate distance between current and next Vertex
    // Calculate curvature using the angle between the vectors from current and next Vertex to the mid Vertex
    float distance = glm::distance(vertex.mPosition, nextVertex.mPosition);
    glm::vec3 v1 = glm::normalize(glm::vec3(vertex.mPosition - midVertex.mPosition));
    glm::vec3 v2 = glm::normalize(glm::vec3(nextVertex.mPosition - midVertex.mPosition));
    float angle = acosf(glm::dot(v1, v2));

    // if the angle is too large, take the opposite value
    if (glm::degrees(angle) > 180.0f)
        angle = glm::radians(360.0f - glm::degrees(angle));

    // Using either curvature or distance, determine if subdivide is necessary
    if ((angle < glm::radians(index) && curvatureBased) || ((distance > index) && !curvatureBased))
    {
        Subdivide(t, mid, p1, p2, p3, p4, vertices, index, curvatureBased);
        Subdivide(mid, nextT, p1, p2, p3, p4, vertices, index, curvatureBased);
    }
    else
    {
        vertices.push_back(vertex);
    }
}