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

    string vertexShaderCodeSKYBOX = OpenGLWindow::CodeShader("skyboxVertex.shader");
    CHECK_STRING(vertexShaderCodeSKYBOX);
    GLuint vertexShaderSKYBOX = OpenGLWindow::CompileShader(vertexShaderCodeSKYBOX, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShaderSKYBOX);

    string vertexShaderCodeInstanced = OpenGLWindow::CodeShader("instancedVertex.shader");
    CHECK_STRING(vertexShaderCodeInstanced);
    GLuint vertexShaderInstanced = OpenGLWindow::CompileShader(vertexShaderCodeInstanced, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShaderInstanced);

    // Fragment shader
    string fragmentShaderCode = OpenGLWindow::CodeShader("fragment.shader");
    CHECK_STRING(fragmentShaderCode);
    GLuint fragmentShader = OpenGLWindow::CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShader);

    string fragmentShaderCodeSKYBOX = OpenGLWindow::CodeShader("skyboxFragment.shader");
    CHECK_STRING(fragmentShaderCodeSKYBOX);
    GLuint fragmentShaderSKYBOX = OpenGLWindow::CompileShader(fragmentShaderCodeSKYBOX, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShaderSKYBOX);

    // Texture Fragment shader
    string textureFragmentShaderCode = OpenGLWindow::CodeShader("textureFragment.shader");
    CHECK_STRING(textureFragmentShaderCode);
    GLuint textureFragmentShader = OpenGLWindow::CompileShader(textureFragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(textureFragmentShader);    

    // Link shaders
    GLuint shaderProgram = OpenGLWindow::AttachShaders(vertexShader, fragmentShader);
    CHECK_ERROR(shaderProgram);

    GLuint shaderProgramSKYBOX = OpenGLWindow::AttachShaders(vertexShaderSKYBOX, fragmentShaderSKYBOX);
    CHECK_ERROR(shaderProgramSKYBOX);

    // Link shaders
    GLuint textureShaderProgram = OpenGLWindow::AttachShaders(vertexShader, textureFragmentShader);
    CHECK_ERROR(textureShaderProgram);

    GLuint instancedShaderProgram = OpenGLWindow::AttachShaders(vertexShaderInstanced, textureFragmentShader);

    OpenGLWindow::SetPointSize(15.0f);
    OpenGLWindow::SetCamera(&mCamera);

    Shape object = ShapeGenerator::GenerateOBJ("Armchair.obj", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    Shape ground = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f));
    Shape wall = ShapeGenerator::GenerateQuad(glm::vec3(1.0f), 10.0f, 20.0f, 2.5f, glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 5.0f));

    //Skybox
    Shape skybox = ShapeGenerator::GenerateCube(glm::vec3(1.0f), glm::vec3(100.0f));

    GLuint OBJECT_VAO, OBJECT_VBO, OBJ_TEXTURE;
    GLuint GROUND_VAO, GROUND_VBO;
    GLuint SKYBOX_VAO, SKYBOX_VBO, SKYBOX_TEXTURE;
    GLuint WALL_VAO, WALL_VBO, WALL_MODEL_MATRIX, WALL_TEXTURE;

    //----Skybox
    glGenVertexArrays(1, &SKYBOX_VAO);
    glGenBuffers(1, &SKYBOX_VBO);
    glGenBuffers(1, &SKYBOX_TEXTURE);

    glBindVertexArray(SKYBOX_VAO);
    OpenGLWindow::BindBuffers(&skybox, &SKYBOX_VBO);

    vector<const GLchar*> faces;
    faces.push_back("right.jpg");
    faces.push_back("left.jpg");
    faces.push_back("top.jpg");
    faces.push_back("bottom.jpg");
    faces.push_back("back.jpg");
    faces.push_back("front.jpg");
    glActiveTexture(GL_TEXTURE0);
    OpenGLWindow::BindSkyboxTexture(&SKYBOX_TEXTURE, faces);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SKYBOX_TEXTURE);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Object
    glGenVertexArrays(1, &OBJECT_VAO);
    glGenBuffers(1, &OBJECT_VBO);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &OBJ_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(OBJECT_VAO);

    OpenGLWindow::BindBuffers(&object, &OBJECT_VBO);
    OpenGLWindow::BindTexture(&OBJ_TEXTURE, "Armchair.jpg");
    OpenGLWindow::AddShape(&object);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Ground
    glGenVertexArrays(1, &GROUND_VAO);
    glGenBuffers(1, &GROUND_VBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(GROUND_VAO);

    OpenGLWindow::BindBuffers(&ground, &GROUND_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Wall
    glGenVertexArrays(1, &WALL_VAO);
    glGenBuffers(1, &WALL_VBO);
    glGenBuffers(1, &WALL_MODEL_MATRIX);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &WALL_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(WALL_VAO);

    vector<glm::mat4> wall_model = OpenGLWindow::GenerateWallModelMatrices();
   OpenGLWindow::BindBuffers(&wall, &WALL_VBO);
   OpenGLWindow::BindModelBuffers(wall_model, &WALL_MODEL_MATRIX);
   OpenGLWindow::BindTexture(&WALL_TEXTURE, "Armchair.jpg");
   // OpenGLWindow::AddShape(&wall);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----End

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set uniform values
    OpenGLWindow::SetUniformFactors(shaderProgram);
    OpenGLWindow::SetUniformFactors(textureShaderProgram);
    OpenGLWindow::SetUniformFactors(instancedShaderProgram);
    glUseProgram(instancedShaderProgram);
    GLuint vpLoc = glGetUniformLocation(instancedShaderProgram, "vp_matrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(OpenGLWindow::GetProjectionMatrix() * mCamera.GetViewMatrix()));

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        OpenGLWindow::DefineViewport(window);

        // Render

        //SKYBOX
        glBindVertexArray(SKYBOX_VAO);
        glDepthMask(GL_FALSE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SKYBOX_TEXTURE);
        OpenGLWindow::SetTexture(shaderProgramSKYBOX, 0, "textureSample");
        OpenGLWindow::RenderShape(&skybox, shaderProgramSKYBOX);
        OpenGLWindow::DrawSkybox(&skybox, &SKYBOX_VBO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);

        //OBJECT
        glBindVertexArray(OBJECT_VAO);
        OpenGLWindow::SetTexture(textureShaderProgram, 0, "textureSample");
        OpenGLWindow::RenderShape(&object, textureShaderProgram);
        OpenGLWindow::DrawShape(&object, &OBJECT_VBO);
        glBindVertexArray(0);

        //GROUND
        glBindVertexArray(GROUND_VAO);
        OpenGLWindow::RenderShape(&ground, shaderProgram);
        OpenGLWindow::DrawShape(&ground, &GROUND_VBO);
        glBindVertexArray(0);

        //WALL
        glBindVertexArray(WALL_VAO);
        glUseProgram(instancedShaderProgram);
        OpenGLWindow::SetTexture(instancedShaderProgram, 0, "textureSample");
        OpenGLWindow::RenderInstancedShape(&wall, instancedShaderProgram);
        OpenGLWindow::DrawInstancedShape(&wall, wall_model.size(), &WALL_VAO, &WALL_VBO);
        glBindVertexArray(0);

        //Swap the screen buffers
        glfwSwapBuffers(window);
    }

    object.CleanUp();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

