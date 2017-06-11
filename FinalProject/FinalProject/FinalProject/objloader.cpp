#include "stdafx.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm.hpp>

#include "objloader.h"

// .OBJ Loader from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// NOTE: objloader code modified from code provided in COMP371 Lab

#pragma warning(disable:4996)

bool loadOBJ(const char * path, vector<Vertex> & out_vertices, glm::vec3 color)
{
    printf("Loading OBJ file %s...\n", path);

    vector<glm::vec3> vertexIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec3> temp_normals;
    vector<glm::vec2> temp_uvs;

    FILE * file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Impossible to open the file ! Are you in the right path ?\n");
        getchar();
        return false;
    }

    while (1)
    {

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            unsigned int vertexIndex[3];
            unsigned int uvIndex[3];
            unsigned int normalIndex[3];

            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                          &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                          &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                          &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            if (matches != 9)
            {
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(glm::vec3(vertexIndex[0], uvIndex[0], normalIndex[0]));
            vertexIndices.push_back(glm::vec3(vertexIndex[1], uvIndex[1], normalIndex[1]));
            vertexIndices.push_back(glm::vec3(vertexIndex[2], uvIndex[2], normalIndex[2]));
        }
        else
        {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i].x;
        unsigned int uvIndex = vertexIndices[i].y;
        unsigned int normalIndex = vertexIndices[i].z;

        // Get the attributes thanks to the index
        //if (vertexIndex > temp_vertices.size())
        //    std::cout << "Vertex " << vertexIndex << std::endl;
        //if (normalIndex > temp_normals.size())
        //    std::cout << "Normal " << normalIndex << std::endl;
        //if (uvIndex > temp_uvs.size())
        //    std::cout << "UV " << uvIndex << std::endl;

        glm::vec3 vertex = temp_vertices[vertexIndex - 1]; //get the correct vertex
        glm::vec3 normal = temp_normals[normalIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];

        // Put the vertex in the std::vector
        out_vertices.push_back(Vertex(vertex, color, normal, uv));
    }

    return true;
}
