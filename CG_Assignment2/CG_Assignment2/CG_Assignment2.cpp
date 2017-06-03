// CG_Assignment2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <glew.h>   // include GL Extension Wrangler
#include <glfw3.h>  // include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>

#include <OpenGLWindow.h>
#include <Camera.h>
#include <Shape.h>
#include <ShapeGenerator.h>

#define CHECK_NULL(a) if (!a) return -1;
#define CHECK_STRING(a) if (a.empty()) return -1;
#define CHECK_INIT(a) if (a != GLEW_OK) return -1;
#define CHECK_ERROR(a) if (a == -1) return -1;
#define SKIP(a) if (a == 1) a++;

#define MAX_POINTS 100

using namespace std;

Camera mCamera;

int main()
{
    OpenGLWindow::InitializeGLFW();

    GLFWwindow* window = OpenGLWindow::CreateWindow();
    CHECK_NULL(window);

    GLenum init = OpenGLWindow::InitializeGLEW();
    CHECK_INIT(init);

    OpenGLWindow::DefineViewport(window);

    // Build and compile our shader program
    // Vertex shader
    string vertexShaderCode = OpenGLWindow::CodeShader("vertex.shader");
    CHECK_STRING(vertexShaderCode);
    GLuint vertexShader = OpenGLWindow::CompileShader(vertexShaderCode, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShader);

    string lightVertexShaderCode = OpenGLWindow::CodeShader("lightVertex.shader");
    CHECK_STRING(lightVertexShaderCode);
    GLuint lightVertexShader = OpenGLWindow::CompileShader(lightVertexShaderCode, GL_VERTEX_SHADER);
    CHECK_ERROR(lightVertexShader);

    string trackVertexShaderCode = OpenGLWindow::CodeShader("trackVertex.shader");
    CHECK_STRING(trackVertexShaderCode);
    GLuint trackVertexShader = OpenGLWindow::CompileShader(trackVertexShaderCode, GL_VERTEX_SHADER);
    CHECK_ERROR(trackVertexShader);

    // Fragment shader
    string fragmentShaderCode = OpenGLWindow::CodeShader("fragment.shader");
    CHECK_STRING(fragmentShaderCode);
    GLuint fragmentShader = OpenGLWindow::CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShader);

    string lightFragmentShaderCode = OpenGLWindow::CodeShader("lightFragment.shader");
    CHECK_STRING(lightFragmentShaderCode);
    GLuint lightFragmentShader = OpenGLWindow::CompileShader(lightFragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(lightFragmentShader);

    // Link shaders
    GLuint shaderProgram = OpenGLWindow::AttachShaders(vertexShader, fragmentShader);
    CHECK_ERROR(shaderProgram);

    GLuint lightShaderProgram = OpenGLWindow::AttachShaders(lightVertexShader, lightFragmentShader);
    CHECK_ERROR(lightShaderProgram);

    GLuint trackShaderProgram = OpenGLWindow::AttachShaders(trackVertexShader, lightFragmentShader);

    OpenGLWindow::SetPointSize(10.0f);
    OpenGLWindow::SetCamera(&mCamera);

set_number:
    int number = 0;
    while (number < 4)
    {
        cout << "Number of points to draw (> 4) : ";
        cin >> number;
    }
    OpenGLWindow::SetMaxPoints(number, window);

    Shape* points = new Shape[number + 1];
    for(int i = 0; i < number + 1; i++)
        points[i] = ShapeGenerator::GeneratePoint(glm::vec2(0.0f), glm::vec3(1.0f));

    Shape spline; // Generated later
    Shape terrain = ShapeGenerator::GenerateTerrain(glm::vec3(0.07f, 0.4f, 0.07f));
    Shape track = ShapeGenerator::GenerateTrack(glm::vec3(0.5f, 0.25f, 0.1f), glm::vec3(0.1f), glm::vec3(0.0f), glm::vec3(0.0f, 0.1f, 0.0f));

    vector<glm::mat4> nullModel;
    vector<glm::vec3> nullOffset;
    vector<glm::vec3> terrainOffset;
    for (float i = -100.0f; i < 100.0f; i++)
        for (float j = -100.0f; j < 100.0f; j++)
            terrainOffset.push_back(glm::vec3(i, 0.0f, j));

    GLuint POINT_VBO;
    GLuint* POINTS = new GLuint[number];
    GLuint SPLINE, SPLINE_VERTEX;
    GLuint TERRAIN, TERRAIN_VERTEX, TERRAIN_OFFSET, TERRAIN_TEXTURE;
    GLuint TRACK, TRACK_VERTEX, TRACK_MODEL_MATRIX, TRACK_TEXTURE;

    glGenVertexArrays(1, &POINT_VBO);
    glGenBuffers(number, POINTS);

    glBindVertexArray(POINT_VBO);

    for (int i = 0; i < number + 1; i++)
    {
        OpenGLWindow::BindBuffers(&points[i], &POINTS[i], false);
        OpenGLWindow::AddShape(&points[i]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &SPLINE);
    glGenBuffers(1, &SPLINE_VERTEX);

    OpenGLWindow::AttachSpline(&spline, &SPLINE);

    glActiveTexture(GL_TEXTURE0);
    glGenVertexArrays(1, &TERRAIN);
    glGenBuffers(1, &TERRAIN_VERTEX);
    glGenBuffers(1, &TERRAIN_OFFSET);
    glGenTextures(1, &TERRAIN_TEXTURE);

    glBindVertexArray(TERRAIN);

    OpenGLWindow::BindBuffers(&terrain, &TERRAIN_VERTEX, true);
    OpenGLWindow::BindOffsetBuffers(terrainOffset, &TERRAIN_OFFSET);
    OpenGLWindow::BindTexture(&TERRAIN_TEXTURE, "grass.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE1);
    glGenVertexArrays(1, &TRACK);
    glGenBuffers(1, &TRACK_VERTEX);
    glGenBuffers(1, &TRACK_MODEL_MATRIX);
    glGenTextures(1, &TRACK_TEXTURE);

    glBindVertexArray(TRACK);

    OpenGLWindow::BindBuffers(&track, &TRACK_VERTEX, true);
    OpenGLWindow::BindModelBuffers(nullModel, &TRACK_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&TRACK_TEXTURE, "brick.jpg");
    OpenGLWindow::AttachTrackModel(&TRACK_MODEL_MATRIX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        OpenGLWindow::DefineViewport(window);

        glEnable(GL_DEPTH_TEST);

        if (OpenGLWindow::mGenerateSpline)
        {
            glBindVertexArray(POINT_VBO);
            for (int i = 0; i < OpenGLWindow::mIndex; i++)
            {
                SKIP(i); // This used to avoid error on 2nd element
                OpenGLWindow::RenderShape(&points[i], shaderProgram);
                OpenGLWindow::DrawShape(&points[i], &POINTS[i]);
            }
            glBindVertexArray(0);

            if (OpenGLWindow::mSplineToggle)
            {
                glBindVertexArray(SPLINE);
                OpenGLWindow::RenderShape(&spline, shaderProgram);
                OpenGLWindow::DrawLines(&spline, &SPLINE);
                glBindVertexArray(0);
            }
        }
        else
        {
            OpenGLWindow::GenerateSpline();
            OpenGLWindow::UpdateCamera();
            OpenGLWindow::SetUniformFactors(lightShaderProgram);

            glBindVertexArray(TERRAIN);
            OpenGLWindow::SetTexture(lightShaderProgram, 0);
            OpenGLWindow::RenderShape(&terrain, lightShaderProgram);
            OpenGLWindow::DrawInstancedShape(&terrain, terrainOffset.size(), &TERRAIN, &TERRAIN_VERTEX);
            glBindVertexArray(0);

            OpenGLWindow::SetUniformFactors(trackShaderProgram);
            OpenGLWindow::SetTexture(trackShaderProgram, 1);
            glBindVertexArray(TRACK);
            OpenGLWindow::RenderTrack(&track, trackShaderProgram);
            OpenGLWindow::DrawInstancedShape(&track, OpenGLWindow::mTrackModel.size(), &TRACK, &TRACK_VERTEX);
            glBindVertexArray(0);
        }

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    points->CleanUp();
    spline.CleanUp();
    terrain.CleanUp();
    track.CleanUp();

    if (!OpenGLWindow::mClose)
        goto set_number;

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

