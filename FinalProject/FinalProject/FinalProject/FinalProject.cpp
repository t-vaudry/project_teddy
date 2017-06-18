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
#include "Particle.h"

#define CHECK_NULL(a) if (!a) return -1;
#define CHECK_STRING(a) if (a.empty()) return -1;
#define CHECK_INIT(a) if (a != GLEW_OK) return -1;
#define CHECK_ERROR(a) if (a == -1) return -1;

using namespace std;

Camera mCamera;

float near_plane = 1.0f;
float far_plane = 50.0f;

GLuint OBJECT_VAO, OBJECT_VBO, OBJ_TEXTURE;
GLuint OBJECT2_VAO, OBJECT2_VBO, OBJ2_TEXTURE;
GLuint FLOOR_VAO, FLOOR_VBO, FLOOR_MODEL_MATRIX, FLOOR_TEXTURE;
GLuint CEILING_VAO, CEILING_VBO, CEILING_MODEL_MATRIX, CEILING_TEXTURE;
GLuint SKYBOX_VAO, SKYBOX_VBO, SKYBOX_TEXTURE;
GLuint WALL_VAO, WALL_VBO, WALL_MODEL_MATRIX, WALL_TEXTURE;
GLuint TOPWINDOW_VAO, TOPWINDOW_VBO, TOPWINDOW_MODEL_MATRIX;
GLuint BOTTOMWINDOW_VAO, BOTTOMWINDOW_VBO, BOTTOMWINDOW_MODEL_MATRIX;
GLuint PARTICLE_VAO, PARTICLE_VBO, PARTICLE_TEXTURE;

//For shadow pass only
GLuint HOUSE_VAO, HOUSE_VBO;

GLuint depthMapFBO;
GLuint depthCubeMap;

GLuint depthMapFBO2;
GLuint depthCubeMap2;

GLuint depthMapFBO3;
GLuint depthCubeMap3;

const unsigned int SHADOW_WIDTH = 1024 * 2, SHADOW_HEIGHT = 1024 * 2;

Shape object;
Shape object2;
Shape wall;
Shape ceiling;
Shape skybox;
Shape mFloor;
Shape topWindow;
Shape bottomWindow;

Shape house;

vector<Particle> particles;

GLuint shaderProgram;
GLuint shaderProgramSKYBOX;
GLuint textureShaderProgram;
GLuint instancedShaderProgram;
GLuint particleShaderProgram;
GLuint shadowShaderProgram;

vector<glm::mat4> topwindow_model;
vector<glm::mat4> bottomwindow_model;
vector<glm::mat4> wall_model;
vector<glm::mat4> floorModelMatrices;
vector<glm::mat4> ceilingModelMatrices;

glm::vec3 lightPos = glm::vec3(28.0f, 0.0f, 28.0f);
glm::vec3 lightPos2 = glm::vec3(33.0f, 0.0f, 28.0f);
glm::vec3 lightPos3 = glm::vec3(28.0f, 0.0f, 33.0f);

int GetUnusedParticleIndex(vector<Particle>& particles)
{
    for (int i = 0; i < NUM_OF_PARTICLES; i++)
    {
        if (particles[i].mLife <= 0.0f)
            return i;
    }

    // Override random if all others are alive
    return (rand() % NUM_OF_PARTICLES);
}

void RenderScene()
{
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

    //FLOOR
    glBindVertexArray(FLOOR_VAO);
    OpenGLWindow::RenderInstancedShape(&mFloor, instancedShaderProgram);
    glUniform1f(glGetUniformLocation(instancedShaderProgram, "far_plane"), far_plane);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    OpenGLWindow::SetTexture(instancedShaderProgram, 7, "depth");
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap2);
    OpenGLWindow::SetTexture(instancedShaderProgram, 8, "depth2");
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap3);
    OpenGLWindow::SetTexture(instancedShaderProgram, 9, "depth3");
    OpenGLWindow::SetTexture(instancedShaderProgram, 2, "textureSample");
    OpenGLWindow::DrawInstancedShape(&mFloor, floorModelMatrices.size(), &FLOOR_VBO);
    glBindVertexArray(0);

    glBindVertexArray(OBJECT2_VAO);
    OpenGLWindow::RenderShape(&object, textureShaderProgram);
    glUniform1f(glGetUniformLocation(textureShaderProgram, "far_plane"), far_plane);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    OpenGLWindow::SetTexture(textureShaderProgram, 7, "depth");
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap2);
    OpenGLWindow::SetTexture(textureShaderProgram, 8, "depth2");
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap3);
    OpenGLWindow::SetTexture(textureShaderProgram, 9, "depth3");
    OpenGLWindow::SetTexture(textureShaderProgram, 6, "textureSample");
    OpenGLWindow::DrawShape(&object, &OBJECT2_VBO);
    glBindVertexArray(0);

    //OBJECTobject2
    glBindVertexArray(OBJECT_VAO);
    OpenGLWindow::RenderShape(&object2, textureShaderProgram);
    glUniform1f(glGetUniformLocation(textureShaderProgram, "far_plane"), far_plane);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    OpenGLWindow::SetTexture(textureShaderProgram, 7, "depth");
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap2);
    OpenGLWindow::SetTexture(textureShaderProgram, 8, "depth2");
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap3);
    OpenGLWindow::SetTexture(textureShaderProgram, 9, "depth3");
    OpenGLWindow::SetTexture(textureShaderProgram, 1, "textureSample");
    OpenGLWindow::DrawShape(&object2, &OBJECT_VBO);
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

    //WINDOWS
    glBindVertexArray(TOPWINDOW_VAO);
    OpenGLWindow::RenderInstancedShape(&topWindow, instancedShaderProgram);
    OpenGLWindow::SetTexture(instancedShaderProgram, 4, "textureSample");
    OpenGLWindow::DrawInstancedShape(&topWindow, topwindow_model.size(), &TOPWINDOW_VBO);
    glBindVertexArray(0);

    glBindVertexArray(BOTTOMWINDOW_VAO);
    OpenGLWindow::RenderInstancedShape(&bottomWindow, instancedShaderProgram);
    OpenGLWindow::SetTexture(instancedShaderProgram, 4, "textureSample");
    OpenGLWindow::DrawInstancedShape(&bottomWindow, bottomwindow_model.size(), &BOTTOMWINDOW_VBO);
    glBindVertexArray(0);

    for (int i = 0; i < SPAWN_PARTICLES; i++)
    {
        int index = GetUnusedParticleIndex(particles);
        particles[index].RespawnParticle();
    }

    for (int i = 0; i < NUM_OF_PARTICLES; i++)
    {
        Particle& p = particles[i];
        p.mLife -= DECAY_FACTOR;

        if (p.mLife > 0.0f)
        {
            p.mInfo.mPosition -= p.mVelocity * DECAY_FACTOR;
            p.mInfo.mAlpha -= DECAY_FACTOR;
        }
    }

    glBindVertexArray(PARTICLE_VAO);
    OpenGLWindow::BindParticleBuffers(particles, &PARTICLE_VBO);
    OpenGLWindow::RenderParticles(particleShaderProgram);
    OpenGLWindow::SetTexture(particleShaderProgram, 5, "tex");
    OpenGLWindow::DrawParticles(particles, &PARTICLE_VBO);
}

void RenderSceneDepth(int room)
{
    //WALLS
    glBindVertexArray(HOUSE_VAO);
    OpenGLWindow::RenderShapeDepth(&house, shadowShaderProgram, room);
    glUniform1f(glGetUniformLocation(shaderProgram, "far_plane"), 50.0f);
    OpenGLWindow::DrawShape(&house, &HOUSE_VBO);
    glBindVertexArray(0);

    //OBJECT
    glBindVertexArray(OBJECT2_VAO);
    OpenGLWindow::RenderShapeDepth(&object, shadowShaderProgram, room);
    glUniform1f(glGetUniformLocation(shaderProgram, "far_plane"), 50.0f);
    OpenGLWindow::DrawShape(&object, &OBJECT2_VBO);
    glBindVertexArray(0);

    glBindVertexArray(OBJECT_VAO);
    OpenGLWindow::RenderShapeDepth(&object2, shadowShaderProgram, room);
    glUniform1f(glGetUniformLocation(shaderProgram, "far_plane"), 50.0f);
    OpenGLWindow::DrawShape(&object2, &OBJECT_VBO);
    glBindVertexArray(0);

    glBindVertexArray(0);
}

void GenerateFBO(int room)
{
    //glGenFramebuffers(1, &depthMapFBO);

    ////Create texture for depth map (with no data since we willload the scene into it)
    GLuint fbo;
    GLuint cubeMap;

    if (room == 1)
    {
        fbo = depthMapFBO;
        cubeMap = depthCubeMap;
        glActiveTexture(GL_TEXTURE7);
    }
    else if (room == 2)
    {
        fbo = depthMapFBO2;
        cubeMap = depthCubeMap2;
        glActiveTexture(GL_TEXTURE8);
    }
    else if (room == 3)
    {
        fbo = depthMapFBO3;
        cubeMap = depthCubeMap3;
        glActiveTexture(GL_TEXTURE9);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //Attach the texture to the frame buffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void DepthPass(int room)
{
    GLuint fbo;
    GLuint cubeMap;
    glm::vec3 light;

    if (room == 1)
    {
        fbo = depthMapFBO;
        light = lightPos;
    }
    else if (room == 2)
    {
        fbo = depthMapFBO2;
        light = lightPos2;
    }
    else if (room == 3)
    {
        fbo = depthMapFBO3;
        light = lightPos3;
    }

    //glm::vec3 lightPos = glm::vec3(28.0f, 1.5f, 30.0f);
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(light, light + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light, light + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light, light + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light, light + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light, light + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light, light + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    //Render the scene into the depth map
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(shadowShaderProgram);
    // --- SENDING VP MATRIX TO GEOMETRY SHADER ----
    for (unsigned int i = 0; i < 6; ++i) {
        string n = "shadowMatrices[" + std::to_string(i) + "]";
        glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, n.c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
    }
    GLuint farPlaneLoc = glGetUniformLocation(shadowShaderProgram, "far_plane");
    glUniform1f(farPlaneLoc, far_plane);

    RenderSceneDepth(room);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// The MAIN function, from here we start the application and run the game loop
int main()
{
    srand(time(NULL));
    OpenGLWindow::InitializeGLFW();

    GLFWwindow* window = OpenGLWindow::CreateWindow();
    CHECK_NULL(window);

    GLenum init = OpenGLWindow::InitializeGLEW();
    CHECK_INIT(init);

    OpenGLWindow::DefineViewport(window);

    // Build and compile our shader program

    // VERTEX

    //--Basic
    string vertexShaderCode = OpenGLWindow::CodeShader("vertex.shader");
    CHECK_STRING(vertexShaderCode);
    GLuint vertexShader = OpenGLWindow::CompileShader(vertexShaderCode, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShader);

    //--Skybox
    string vertexShaderCodeSKYBOX = OpenGLWindow::CodeShader("skyboxVertex.shader");
    CHECK_STRING(vertexShaderCodeSKYBOX);
    GLuint vertexShaderSKYBOX = OpenGLWindow::CompileShader(vertexShaderCodeSKYBOX, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShaderSKYBOX);

    //--Instanced
    string vertexShaderCodeInstanced = OpenGLWindow::CodeShader("instancedVertex.shader");
    CHECK_STRING(vertexShaderCodeInstanced);
    GLuint vertexShaderInstanced = OpenGLWindow::CompileShader(vertexShaderCodeInstanced, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShaderInstanced);

    //--Particle system
    string vertexShaderCodeParticle = OpenGLWindow::CodeShader("particleVertex.shader");
    CHECK_STRING(vertexShaderCodeParticle);
    GLuint vertexShaderParticle = OpenGLWindow::CompileShader(vertexShaderCodeParticle, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShaderParticle);

    //--Shadows
    string vertexShaderCodeShadows = OpenGLWindow::CodeShader("depth_vertex.shader");
    CHECK_STRING(vertexShaderCodeShadows);
    GLuint vertexShaderShadows = OpenGLWindow::CompileShader(vertexShaderCodeShadows, GL_VERTEX_SHADER);
    CHECK_ERROR(vertexShaderShadows);

    // FRAGMENT

    //--Basic
    string fragmentShaderCode = OpenGLWindow::CodeShader("fragment.shader");
    CHECK_STRING(fragmentShaderCode);
    GLuint fragmentShader = OpenGLWindow::CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShader);

    //--Skybox
    string fragmentShaderCodeSKYBOX = OpenGLWindow::CodeShader("skyboxFragment.shader");
    CHECK_STRING(fragmentShaderCodeSKYBOX);
    GLuint fragmentShaderSKYBOX = OpenGLWindow::CompileShader(fragmentShaderCodeSKYBOX, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShaderSKYBOX);

    //--Textures
    string textureFragmentShaderCode = OpenGLWindow::CodeShader("textureFragment.shader");
    CHECK_STRING(textureFragmentShaderCode);
    GLuint textureFragmentShader = OpenGLWindow::CompileShader(textureFragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(textureFragmentShader);

    //--Particle system
    string particleFragmentShaderCode = OpenGLWindow::CodeShader("particleFragment.shader");
    CHECK_STRING(particleFragmentShaderCode);
    GLuint particleFragmentShader = OpenGLWindow::CompileShader(particleFragmentShaderCode, GL_FRAGMENT_SHADER);
    CHECK_ERROR(particleFragmentShader);

    //--Shadows
    string fragmentShaderCodeShadows = OpenGLWindow::CodeShader("depth_fragment.shader");
    CHECK_STRING(fragmentShaderCodeShadows);
    GLuint fragmentShaderShadows = OpenGLWindow::CompileShader(fragmentShaderCodeShadows, GL_FRAGMENT_SHADER);
    CHECK_ERROR(fragmentShaderShadows);

    // GEOMETRY

    //--Shadows
    string geometryShaderCodeShadows = OpenGLWindow::CodeShader("depth_geometry.shader");
    CHECK_STRING(geometryShaderCodeShadows);
    GLuint geometryShaderShadows = OpenGLWindow::CompileShader(geometryShaderCodeShadows, GL_GEOMETRY_SHADER);
    CHECK_ERROR(geometryShaderShadows);

    // Link shaders

    //--Basic
    shaderProgram = OpenGLWindow::AttachShaders(vertexShader, fragmentShader);
    CHECK_ERROR(shaderProgram);

    //--Skybox
    shaderProgramSKYBOX = OpenGLWindow::AttachShaders(vertexShaderSKYBOX, fragmentShaderSKYBOX);
    CHECK_ERROR(shaderProgramSKYBOX);

    //--Texture
    textureShaderProgram = OpenGLWindow::AttachShaders(vertexShader, textureFragmentShader);
    CHECK_ERROR(textureShaderProgram);

    //--Instanced
    instancedShaderProgram = OpenGLWindow::AttachShaders(vertexShaderInstanced, textureFragmentShader);
    CHECK_ERROR(instancedShaderProgram);

    //--Particle System
    particleShaderProgram = OpenGLWindow::AttachShaders(vertexShaderParticle, particleFragmentShader);
    CHECK_ERROR(particleShaderProgram);

    //--Shadows
    shadowShaderProgram = OpenGLWindow::AttachShaders(vertexShaderShadows, fragmentShaderShadows, geometryShaderShadows);
    CHECK_ERROR(particleShaderProgram);

    OpenGLWindow::SetPointSize(5.0f);
    OpenGLWindow::SetCamera(&mCamera);

    //Shape object = ShapeGenerator::GenerateOBJ("Sofa.obj", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.75f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(22.0f, 0.25f, 27.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("burlap_sofa.obj", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(26.0f, -0.75f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("kitchen_table.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -0.75f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("kitchen_chair.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -0.5f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("shelves.obj", glm::vec3(0.0f), glm::vec3(0.1f), glm::vec3(0.0f), glm::vec3(26.0f, -0.75f, 26.0f));
    //cube = ShapeGenerator::GenerateCube(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(30.0f, 0.0f, 30.0f));
    //cube = ShapeGenerator::GenerateOBJ("desk.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(33.0f, 0.0f, 28.0f));
    //cubeFloor = ShapeGenerator::GenerateOBJ("cube.obj", glm::vec3(0.0f), glm::vec3(20, 0.01, 10), glm::vec3(0.0f), glm::vec3(25.0f, -1.0f, 25.0f));

    house = ShapeGenerator::GenerateOBJ("House.obj", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f, -90.0f, 180.0f), glm::vec3(30.6357574f, -1.0f, 30.0619736f));
    object2 = ShapeGenerator::GenerateOBJ("Recliner.obj", glm::vec3(0.0f), glm::vec3(0.75f), glm::vec3(0.0f), glm::vec3(30.0f, -0.5f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("bed.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -1.0f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("dresser.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -0.5f, 26.0f));
    object = ShapeGenerator::GenerateOBJ("desk.obj", glm::vec3(0.0f), glm::vec3(0.1f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(26.0f, 0.0f, 26.0f));

    mFloor = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f), -1.0f, false);
    ceiling = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f), 1.0f, true);

    ShapeGenerator::SetWallDimensions(1.0f, 2.0f, 0.25f);
    wall = ShapeGenerator::GenerateWall();
    topWindow = ShapeGenerator::GenerateWindow();
    bottomWindow = ShapeGenerator::GenerateWindow();

    //Skybox
    skybox = ShapeGenerator::GenerateCube(glm::vec3(1.0f), glm::vec3(100.0f));

    for (int i = 0; i < NUM_OF_PARTICLES; i++)
        particles.push_back(Particle());

    floorModelMatrices = ShapeGenerator::GeneratePlaneModelMatrices(&mFloor);
    ceilingModelMatrices = ShapeGenerator::GeneratePlaneModelMatrices(&ceiling);

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

    //----House (for shadow pass only)
    glGenVertexArrays(1, &HOUSE_VAO);
    glGenBuffers(1, &HOUSE_VBO);

    glBindVertexArray(HOUSE_VAO);

    OpenGLWindow::BindBuffers(&house, &HOUSE_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Object
    glGenVertexArrays(1, &OBJECT_VAO);
    glGenBuffers(1, &OBJECT_VBO);

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &OBJ_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(OBJECT_VAO);

    OpenGLWindow::BindBuffers(&object2, &OBJECT_VBO);
    OpenGLWindow::BindTexture(&OBJ_TEXTURE, "leather.jpg");
    OpenGLWindow::AddShape(&object2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Object
    glGenVertexArrays(1, &OBJECT2_VAO);
    glGenBuffers(1, &OBJECT2_VBO);

    glActiveTexture(GL_TEXTURE6);
    glGenTextures(1, &OBJ2_TEXTURE);

    glBindVertexArray(OBJECT2_VAO);

    OpenGLWindow::BindBuffers(&object, &OBJECT2_VBO);
    OpenGLWindow::BindTexture(&OBJ2_TEXTURE, "light_wood.jpg");
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

    OpenGLWindow::BindBuffers(&mFloor, &FLOOR_VBO);
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

    wall_model = ShapeGenerator::GenerateWallModelMatrices();
    OpenGLWindow::BindBuffers(&wall, &WALL_VBO);
    OpenGLWindow::BindModelBuffers(wall_model, &WALL_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&WALL_TEXTURE, "wallpaper.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Wall (top of window)
    glGenVertexArrays(1, &TOPWINDOW_VAO);
    glGenBuffers(1, &TOPWINDOW_VBO);
    glGenBuffers(1, &TOPWINDOW_MODEL_MATRIX);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(TOPWINDOW_VAO);

    topwindow_model = ShapeGenerator::GetTopWindowMatrices();
    OpenGLWindow::BindBuffers(&topWindow, &TOPWINDOW_VBO);
    OpenGLWindow::BindModelBuffers(topwindow_model, &TOPWINDOW_MODEL_MATRIX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Wall (bottom of window)
    glGenVertexArrays(1, &BOTTOMWINDOW_VAO);
    glGenBuffers(1, &BOTTOMWINDOW_VBO);
    glGenBuffers(1, &BOTTOMWINDOW_MODEL_MATRIX);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(BOTTOMWINDOW_VAO);

    bottomwindow_model = ShapeGenerator::GetBottomWindowMatrices();
    OpenGLWindow::BindBuffers(&bottomWindow, &BOTTOMWINDOW_VBO);
    OpenGLWindow::BindModelBuffers(bottomwindow_model, &BOTTOMWINDOW_MODEL_MATRIX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE5);
    glGenVertexArrays(1, &PARTICLE_VAO);
    glGenBuffers(1, &PARTICLE_VBO);
    glGenTextures(1, &PARTICLE_TEXTURE);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(PARTICLE_VAO);

    OpenGLWindow::BindParticleBuffers(particles, &PARTICLE_VBO);
    OpenGLWindow::BindTexture(&PARTICLE_TEXTURE, "fire.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----End

    //----Depth map frame buffer object

    glGenFramebuffers(1, &depthMapFBO);
    glActiveTexture(GL_TEXTURE7);
    glGenTextures(1, &depthCubeMap);
    GenerateFBO(1);

    glGenFramebuffers(1, &depthMapFBO2);
    glActiveTexture(GL_TEXTURE8);
    glGenTextures(1, &depthCubeMap2);
    GenerateFBO(2);


    glGenFramebuffers(1, &depthMapFBO3);
    glActiveTexture(GL_TEXTURE9);
    glGenTextures(1, &depthCubeMap3);
    GenerateFBO(3);

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
        OpenGLWindow::JoystickCallback();

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        OpenGLWindow::DefineViewport(window);

        //--Depth pass
        DepthPass(1);
        DepthPass(2);
        DepthPass(3);

        glViewport(0, 0, WIDTH, HEIGHT);
        OpenGLWindow::DefineViewport(window);

        RenderScene();

        //Swap the screen buffers
        glfwSwapBuffers(window);
    }

    object.CleanUp();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

