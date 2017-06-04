#include "stdafx.h"

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

    vector<unsigned int> vertexIndices;
    vector<glm::vec3> temp_vertices;

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
        else if (strcmp(lineHeader, "f") == 0)
        {
            unsigned int vertexIndex[3];

            int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

            if (matches != 3)
            {
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
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
        unsigned int vertexIndex = vertexIndices[i];

        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[vertexIndex - 1]; //get the correct vertex

                                                           // Put the vertex in the std::vector
        out_vertices.push_back(Vertex(vertex, color));
    }

    return true;
}
