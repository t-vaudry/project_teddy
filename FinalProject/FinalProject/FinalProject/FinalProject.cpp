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

    // Color Fragment shader
    string fragmentShaderCode = OpenGLWindow::CodeShader("fragment.shader");
    CHECK_STRING(fragmentShaderCode);
    GLuint fragmentShader = OpenGLWindow::CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShader);

    // Texture Fragment shader
    string textureFragmentShaderCode = OpenGLWindow::CodeShader("textureFragment.shader");
    CHECK_STRING(textureFragmentShaderCode);
    GLuint textureFragmentShader = OpenGLWindow::CompileShader(textureFragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(textureFragmentShader);

    // Link shaders
    GLuint shaderProgram = OpenGLWindow::AttachShaders(vertexShader, fragmentShader);
    CHECK_ERROR(shaderProgram);

    // Link shaders
    GLuint textureShaderProgram = OpenGLWindow::AttachShaders(vertexShader, textureFragmentShader);
    CHECK_ERROR(textureShaderProgram);

    OpenGLWindow::SetPointSize(15.0f);
    OpenGLWindow::SetCamera(&mCamera);

    Shape object = ShapeGenerator::GenerateOBJ("Armchair.obj", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    Shape ground = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f));

    GLuint OBJECT_VAO, OBJECT_VBO, OBJ_TEXTURE;
    GLuint GROUND_VAO, GROUND_VBO;

    glActiveTexture(GL_TEXTURE0);
    glGenVertexArrays(1, &OBJECT_VAO);
    glGenBuffers(1, &OBJECT_VBO);
    glGenTextures(1, &OBJ_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(OBJECT_VAO);

    OpenGLWindow::BindBuffers(&object, &OBJECT_VBO);
    OpenGLWindow::BindTexture(&OBJ_TEXTURE, "Armchair.jpg");
    OpenGLWindow::AddShape(&object);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &GROUND_VAO);
    glGenBuffers(1, &GROUND_VBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(GROUND_VAO);

    OpenGLWindow::BindBuffers(&ground, &GROUND_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set uniform values
    OpenGLWindow::SetUniformFactors(shaderProgram);
    OpenGLWindow::SetUniformFactors(textureShaderProgram);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        OpenGLWindow::DefineViewport(window);

        glBindVertexArray(OBJECT_VAO);

        // Render
        OpenGLWindow::SetTexture(textureShaderProgram, 0, "textureSample");
        OpenGLWindow::RenderShape(&object, textureShaderProgram);
        OpenGLWindow::DrawShape(&object, &OBJECT_VBO);

        glBindVertexArray(0);
        glBindVertexArray(GROUND_VAO);

        OpenGLWindow::RenderShape(&ground, shaderProgram);
        OpenGLWindow::DrawShape(&ground, &GROUND_VBO);

        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    object.CleanUp();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

