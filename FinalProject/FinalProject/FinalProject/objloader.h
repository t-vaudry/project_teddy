#pragma once
#include <vector>
#include <Vertex.h>
#include <glew.h>

using namespace std;

bool loadOBJ(
    const char * path,
    vector<Vertex> & out_vertices,
    glm::vec3 color
    );