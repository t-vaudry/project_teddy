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
    CHECK_ERROR(instancedShaderProgram);

    OpenGLWindow::SetPointSize(15.0f);
    OpenGLWindow::SetCamera(&mCamera);

    Shape object = ShapeGenerator::GenerateOBJ("table.obj", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.25f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(28.0f, -1.0f, 26.0f));
    Shape floor = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f), -1.0f, false);
    Shape ceiling = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f), 1.0f, true);

    ShapeGenerator::SetWallDimensions(1.0f, 2.0f, 0.25f);
    Shape wall = ShapeGenerator::GenerateWall();

    //Skybox
    Shape skybox = ShapeGenerator::GenerateCube(glm::vec3(1.0f), glm::vec3(100.0f));

    vector<glm::mat4> floorModelMatrices = ShapeGenerator::GeneratePlaneModelMatrices(&floor);
    vector<glm::mat4> ceilingModelMatrices = ShapeGenerator::GeneratePlaneModelMatrices(&ceiling);

    GLuint OBJECT_VAO, OBJECT_VBO, OBJ_TEXTURE;
    GLuint FLOOR_VAO, FLOOR_VBO, FLOOR_MODEL_MATRIX, FLOOR_TEXTURE;
    GLuint CEILING_VAO, CEILING_VBO, CEILING_MODEL_MATRIX, CEILING_TEXTURE;
    GLuint SKYBOX_VAO, SKYBOX_VBO, SKYBOX_TEXTURE;
    GLuint WALL_VAO, WALL_VBO, WALL_MODEL_MATRIX, WALL_TEXTURE;

    // DEBUG
    GLuint POINT_VAO, POINT_VBO;

    //----Skybox
    glGenVertexArrays(1, &SKYBOX_VAO);
    glGenBuffers(1, &SKYBOX_VBO);
    glGenTextures(1, &SKYBOX_TEXTURE);

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

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &OBJ_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(OBJECT_VAO);

    OpenGLWindow::BindBuffers(&object, &OBJECT_VBO);
    OpenGLWindow::BindTexture(&OBJ_TEXTURE, "BedsideTable.png");
    OpenGLWindow::AddShape(&object);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----FLOOR
    glGenVertexArrays(1, &FLOOR_VAO);
    glGenBuffers(1, &FLOOR_VBO);
    glGenBuffers(1, &FLOOR_MODEL_MATRIX);

    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &FLOOR_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(FLOOR_VAO);

    OpenGLWindow::BindBuffers(&floor, &FLOOR_VBO);
    OpenGLWindow::BindModelBuffers(floorModelMatrices, &FLOOR_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&FLOOR_TEXTURE, "floor.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----CEILING
    glGenVertexArrays(1, &CEILING_VAO);
    glGenBuffers(1, &CEILING_VBO);
    glGenBuffers(1, &CEILING_MODEL_MATRIX);

    glActiveTexture(GL_TEXTURE3);
    glGenTextures(1, &CEILING_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(CEILING_VAO);

    OpenGLWindow::BindBuffers(&ceiling, &CEILING_VBO);
    OpenGLWindow::BindModelBuffers(ceilingModelMatrices, &CEILING_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&CEILING_TEXTURE, "ceiling.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Wall
    glGenVertexArrays(1, &WALL_VAO);
    glGenBuffers(1, &WALL_VBO);
    glGenBuffers(1, &WALL_MODEL_MATRIX);

    glActiveTexture(GL_TEXTURE4);
    glGenTextures(1, &WALL_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(WALL_VAO);

    vector<glm::mat4> wall_model = ShapeGenerator::GenerateWallModelMatrices();
   OpenGLWindow::BindBuffers(&wall, &WALL_VBO);
   OpenGLWindow::BindModelBuffers(wall_model, &WALL_MODEL_MATRIX);
   OpenGLWindow::BindTexture(&WALL_TEXTURE, "wallpaper.jpg");

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
        OpenGLWindow::RenderShape(&skybox, shaderProgramSKYBOX);
        OpenGLWindow::SetTexture(shaderProgramSKYBOX, 0, "textureSample");
        OpenGLWindow::DrawSkybox(&skybox, &SKYBOX_VBO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);

        //OBJECT
        glBindVertexArray(OBJECT_VAO);
        OpenGLWindow::RenderShape(&object, textureShaderProgram);
        OpenGLWindow::SetTexture(textureShaderProgram, 1, "textureSample");
        OpenGLWindow::DrawShape(&object, &OBJECT_VBO);
        glBindVertexArray(0);

        //FLOOR
        glBindVertexArray(FLOOR_VAO);
        OpenGLWindow::RenderInstancedShape(&floor, instancedShaderProgram);
        OpenGLWindow::SetTexture(instancedShaderProgram, 2, "textureSample");
        OpenGLWindow::DrawInstancedShape(&floor, floorModelMatrices.size(), &FLOOR_VBO);
        glBindVertexArray(0);

        //CEILING
        glBindVertexArray(CEILING_VAO);
        OpenGLWindow::RenderInstancedShape(&ceiling, instancedShaderProgram);
        OpenGLWindow::SetTexture(instancedShaderProgram, 3, "textureSample");
        OpenGLWindow::DrawInstancedShape(&ceiling, ceilingModelMatrices.size(), &CEILING_VBO);
        glBindVertexArray(0);

        //WALL
        glBindVertexArray(WALL_VAO);
        OpenGLWindow::RenderInstancedShape(&wall, instancedShaderProgram);
        OpenGLWindow::SetTexture(instancedShaderProgram, 4, "textureSample");
        OpenGLWindow::DrawInstancedShape(&wall, wall_model.size(), &WALL_VBO);
        glBindVertexArray(0);

        //Swap the screen buffers
        glfwSwapBuffers(window);
    }

    object.CleanUp();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

