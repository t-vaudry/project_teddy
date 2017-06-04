// CG_Assignment1.cpp : Defines the entry point for the console application.
#include "stdafx.h"

#include <glew.h>   // include GL Extension Wrangler
#include <glfw3.h>  // include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>

#include "OpenGLWindow.h"
#include "Camera.h"
#include "Shape.h"
#include "ShapeGenerator.h"

#define CHECK_NULL(a) if (!a) return -1;
#define CHECK_STRING(a) if (a.empty()) return -1;
#define CHECK_INIT(a) if (a != GLEW_OK) return -1;
#define CHECK_ERROR(a) if (a == -1) return -1;

using namespace std;

Camera mCamera;

// The MAIN function, from here we start the application and run the game loop
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

    // Fragment shader
    string fragmentShaderCode = OpenGLWindow::CodeShader("fragment.shader");
    CHECK_STRING(fragmentShaderCode);
    GLuint fragmentShader = OpenGLWindow::CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShader);

    // Link shaders
    GLuint shaderProgram = OpenGLWindow::AttachShaders(vertexShader, fragmentShader);
    CHECK_ERROR(shaderProgram);

    OpenGLWindow::SetPointSize(15.0f);
    OpenGLWindow::SetCamera(&mCamera);

    glm::vec3 origin = glm::vec3(0.0f);

    Shape teddySpinner = ShapeGenerator::GenerateTeddy(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.1f));
    Shape teddyRevolver = ShapeGenerator::GenerateTeddy(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.05f), glm::vec3(0.0f), glm::vec3(-10.0f, 0.0f, 0.0f));
    Shape spinner = ShapeGenerator::GenerateCube(glm::vec3(0.0f, 0.0f, 1.0f));
    Shape revolver = ShapeGenerator::GenerateCube(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -10.0f));
    Shape xAxis = ShapeGenerator::GenerateLine(glm::vec3(8.0f, 0.0f, 0.0f), origin, glm::vec3(1.0f, 0.0f, 0.0f));
    Shape yAxis = ShapeGenerator::GenerateLine(glm::vec3(0.0f, 8.0f, 0.0f), origin, glm::vec3(0.0f, 0.0f, 0.0f));
    Shape zAxis = ShapeGenerator::GenerateLine(glm::vec3(0.0f, 0.0f, 8.0f), origin, glm::vec3(0.0f, 1.0f, 0.0f));
    Shape line = ShapeGenerator::GenerateLine(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    GLuint VAO, SPINNER, REVOLVER, X_AXIS, Y_AXIS, Z_AXIS, LINE, TEDDY_SPINNER, TEDDY_REVOLVER;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &SPINNER);
    glGenBuffers(1, &REVOLVER);
    glGenBuffers(1, &X_AXIS);
    glGenBuffers(1, &Y_AXIS);
    glGenBuffers(1, &Z_AXIS);
    glGenBuffers(1, &LINE);
    glGenBuffers(1, &TEDDY_SPINNER);
    glGenBuffers(1, &TEDDY_REVOLVER);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    OpenGLWindow::BindBuffers(&spinner, &SPINNER);
    OpenGLWindow::AddShape(&spinner);

    OpenGLWindow::BindBuffers(&revolver, &REVOLVER);
    OpenGLWindow::AddShape(&revolver);

    OpenGLWindow::BindBuffers(&xAxis, &X_AXIS);
    OpenGLWindow::AddShape(&xAxis);

    OpenGLWindow::BindBuffers(&yAxis, &Y_AXIS);
    OpenGLWindow::AddShape(&yAxis);

    OpenGLWindow::BindBuffers(&zAxis, &Z_AXIS);
    OpenGLWindow::AddShape(&zAxis);

    OpenGLWindow::BindBuffers(&line, &LINE);
    OpenGLWindow::AddShape(&line);

    OpenGLWindow::BindBuffers(&teddySpinner, &TEDDY_SPINNER);
    OpenGLWindow::AddShape(&teddySpinner);

    OpenGLWindow::BindBuffers(&teddyRevolver, &TEDDY_REVOLVER);
    OpenGLWindow::AddShape(&teddyRevolver);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        // Clear the color and depth buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Redefine the viewport for resizing
        OpenGLWindow::DefineViewport(window);

        glBindVertexArray(VAO);
        glEnable(GL_DEPTH_TEST);

        // Render
        if (!OpenGLWindow::mTeddyToggle)
        {
            OpenGLWindow::RenderShape(&spinner, shaderProgram);
            OpenGLWindow::DrawShape(&spinner, &SPINNER);
        }
        else
        {
            OpenGLWindow::RenderShape(&teddySpinner, shaderProgram);
            OpenGLWindow::DrawShape(&teddySpinner, &TEDDY_SPINNER);
        }

        if (OpenGLWindow::mAnimationToggle)
        {
            spinner.mRotate.y += ROTATION_STEP;
            teddySpinner.mRotate.y += ROTATION_STEP;
        }

        if (!OpenGLWindow::mTeddyToggle)
        {
            OpenGLWindow::RenderShape(&revolver, shaderProgram);
            OpenGLWindow::DrawShape(&revolver, &REVOLVER);
        }
        else
        {
            OpenGLWindow::RenderShape(&teddyRevolver, shaderProgram);
            OpenGLWindow::DrawShape(&teddyRevolver, &TEDDY_REVOLVER);
        }

        if (OpenGLWindow::mAnimationToggle)
        {
            revolver.mRotate.y += ROTATION_STEP;
            teddyRevolver.mRotate.y += ROTATION_STEP;
        }

        if (OpenGLWindow::mSinesoidToggle)
        {
            revolver.mTranslate.x = (-8.0f + 2 * sinf(OpenGLWindow::mWaves*glm::radians(revolver.mRotate.y)));
            revolver.mTranslate.z = (-8.0f + 2 * sinf(OpenGLWindow::mWaves*glm::radians(revolver.mRotate.y)));
            teddyRevolver.mTranslate.x = (-8.0f + 2 * sinf(OpenGLWindow::mWaves*glm::radians(teddyRevolver.mRotate.y)));
            teddyRevolver.mTranslate.z = (-8.0f + 2 * sinf(OpenGLWindow::mWaves*glm::radians(teddyRevolver.mRotate.y)));
        }

        OpenGLWindow::RenderShape(&xAxis, shaderProgram);
        OpenGLWindow::DrawLines(&xAxis, &X_AXIS);

        OpenGLWindow::RenderShape(&yAxis, shaderProgram);
        OpenGLWindow::DrawLines(&yAxis, &Y_AXIS);

        OpenGLWindow::RenderShape(&zAxis, shaderProgram);
        OpenGLWindow::DrawLines(&zAxis, &Z_AXIS);

        OpenGLWindow::RenderShape(&line, shaderProgram);
        OpenGLWindow::DrawLines(&line, &LINE);

        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    spinner.CleanUp();
    revolver.CleanUp();
    teddySpinner.CleanUp();
    teddyRevolver.CleanUp();
    xAxis.CleanUp();
    yAxis.CleanUp();
    zAxis.CleanUp();
    line.CleanUp();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

