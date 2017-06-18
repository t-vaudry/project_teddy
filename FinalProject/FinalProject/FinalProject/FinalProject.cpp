// CG_Assignment1.cpp : Defines the entry point for the console application.
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

using namespace std;

Camera mCamera;

float near_plane = 1.0f;
float far_plane = 50.0f;

//Buffer objects
GLuint RECLINER_VAO, RECLINER_VBO, RECLINER_TEXTURE;
GLuint DESK_VAO, DESK_VBO, DESK_TEXTURE;
GLuint TABLE_VAO, TABLE_VBO, TABLE_TEXTURE;
GLuint FLOOR_VAO, FLOOR_VBO, FLOOR_MODEL_MATRIX, FLOOR_TEXTURE;
GLuint CEILING_VAO, CEILING_VBO, CEILING_MODEL_MATRIX, CEILING_TEXTURE;
GLuint SKYBOX_VAO, SKYBOX_VBO, SKYBOX_TEXTURE;
GLuint WALL_VAO, WALL_VBO, WALL_MODEL_MATRIX, WALL_TEXTURE;
GLuint TOPWINDOW_VAO, TOPWINDOW_VBO, TOPWINDOW_MODEL_MATRIX;
GLuint BOTTOMWINDOW_VAO, BOTTOMWINDOW_VBO, BOTTOMWINDOW_MODEL_MATRIX;
GLuint WINDOW_VAO, WINDOW_VBO, WINDOW_MODEL_MATRIX, WINDOW_TEXTURE;
GLuint PARTICLE_VAO, PARTICLE_VBO, PARTICLE_TEXTURE;
GLuint CROSSHAIR_VAO, CROSSHAIR_VBO;
GLuint MENU_VAO, MENU_VBO, MENU_TEXTURE;

//For shadow pass only
GLuint HOUSE_VAO, HOUSE_VBO;

GLuint mDepthMapFBO;
GLuint mDepthCubeMap;

GLuint mDepthMapFBO2;
GLuint mDepthCubeMap2;

GLuint mDepthMapFBO3;
GLuint mDepthCubeMap3;

const unsigned int SHADOW_WIDTH = 1024 * 2, SHADOW_HEIGHT = 1024 * 2;

//Shapes
Shape mDesk;
Shape mRecliner;
Shape mTable;

Shape mWall;
Shape mCeiling;
Shape mSkybox;
Shape mFloor;

Shape mTopWindow;
Shape mBottomWindow;
Shape mWindows;
Shape mCrosshair;
Shape mMenu;

Shape mHouse;

vector<Particle> mParticles;

//Shader programs
GLuint mShaderProgram;
GLuint mSkyboxShaderProgram;
GLuint mTextureShaderProgram;
GLuint mInstancedShaderProgram;
GLuint mParticleShaderProgram;
GLuint mShadowShaderProgram;

//Model matrices for instanced programs
vector<glm::mat4> mTopWindowModelMatrices;
vector<glm::mat4> mBottomWindowModelMatrices;
vector<glm::mat4> mWallModelMatrices;
vector<glm::mat4> mFloorModelMatrices;
vector<glm::mat4> mCeilingModelMatrices;
vector<glm::mat4> mWindowModelMatrices;

//Positions of light sources
glm::vec3 lightPos = glm::vec3(28.0f, 0.0f, 28.0f);
glm::vec3 lightPos2 = glm::vec3(33.0f, 0.0f, 28.0f);
glm::vec3 lightPos3 = glm::vec3(28.0f, 0.0f, 33.0f);

//-----Get index of available particle
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

//-----Render all objects in the scene
void RenderScene()
{
    //SKYBOX
    glBindVertexArray(SKYBOX_VAO);
    glDepthMask(GL_FALSE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SKYBOX_TEXTURE);
    OpenGLWindow::RenderShape(&mSkybox, mSkyboxShaderProgram);
    OpenGLWindow::SetTexture(mSkyboxShaderProgram, 0, "textureSample");
    OpenGLWindow::DrawSkybox(&mSkybox, &SKYBOX_VBO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);

    //FLOOR
    glBindVertexArray(FLOOR_VAO);
    OpenGLWindow::RenderInstancedShape(&mFloor, mInstancedShaderProgram);
    glUniform1f(glGetUniformLocation(mInstancedShaderProgram, "far_plane"), far_plane);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 7, "depth");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap2);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 8, "depth2");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap3);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 9, "depth3");
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 2, "textureSample");
    OpenGLWindow::DrawInstancedShape(&mFloor, mFloorModelMatrices.size(), &FLOOR_VBO);
    glBindVertexArray(0);

    //DESK
    glBindVertexArray(DESK_VAO);
    OpenGLWindow::RenderShape(&mDesk, mTextureShaderProgram);
    glUniform1f(glGetUniformLocation(mTextureShaderProgram, "far_plane"), far_plane);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 7, "depth");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap2);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 8, "depth2");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap3);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 9, "depth3");
    OpenGLWindow::SetTexture(mTextureShaderProgram, 6, "textureSample");
    OpenGLWindow::DrawShape(&mDesk, &DESK_VBO);
    glBindVertexArray(0);

    //TABLE
    glBindVertexArray(TABLE_VAO);
    OpenGLWindow::RenderShape(&mTable, mTextureShaderProgram);
    glUniform1f(glGetUniformLocation(mTextureShaderProgram, "far_plane"), far_plane);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 7, "depth");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap2);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 8, "depth2");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap3);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 9, "depth3");
    OpenGLWindow::SetTexture(mTextureShaderProgram, 6, "textureSample");
    OpenGLWindow::DrawShape(&mTable, &TABLE_VBO);
    glBindVertexArray(0);

    //RECLINER
    glBindVertexArray(RECLINER_VAO);
    OpenGLWindow::RenderShape(&mRecliner, mTextureShaderProgram);
    glUniform1f(glGetUniformLocation(mTextureShaderProgram, "far_plane"), far_plane);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 7, "depth");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap2);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 8, "depth2");
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDepthCubeMap3);
    OpenGLWindow::SetTexture(mTextureShaderProgram, 9, "depth3");
    OpenGLWindow::SetTexture(mTextureShaderProgram, 1, "textureSample");
    OpenGLWindow::DrawShape(&mRecliner, &RECLINER_VBO);
    glBindVertexArray(0);

    //CEILING
    glBindVertexArray(CEILING_VAO);
    OpenGLWindow::RenderInstancedShape(&mCeiling, mInstancedShaderProgram);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 3, "textureSample");
    OpenGLWindow::DrawInstancedShape(&mCeiling, mCeilingModelMatrices.size(), &CEILING_VBO);
    glBindVertexArray(0);

    //WALL
    glBindVertexArray(WALL_VAO);
    OpenGLWindow::RenderInstancedShape(&mWall, mInstancedShaderProgram);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 4, "textureSample");
    OpenGLWindow::DrawInstancedShape(&mWall, mWallModelMatrices.size(), &WALL_VBO);
    glBindVertexArray(0);

    //WINDOWS
    glBindVertexArray(TOPWINDOW_VAO);
    OpenGLWindow::RenderInstancedShape(&mTopWindow, mInstancedShaderProgram);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 4, "textureSample");
    OpenGLWindow::DrawInstancedShape(&mTopWindow, mTopWindowModelMatrices.size(), &TOPWINDOW_VBO);
    glBindVertexArray(0);

    glBindVertexArray(BOTTOMWINDOW_VAO);
    OpenGLWindow::RenderInstancedShape(&mBottomWindow, mInstancedShaderProgram);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 4, "textureSample");
    OpenGLWindow::DrawInstancedShape(&mBottomWindow, mBottomWindowModelMatrices.size(), &BOTTOMWINDOW_VBO);
    glBindVertexArray(0);

    glBindVertexArray(WINDOW_VAO);
    OpenGLWindow::RenderInstancedShape(&mWindows, mInstancedShaderProgram);
    OpenGLWindow::SetTexture(mInstancedShaderProgram, 11, "textureSample");
    OpenGLWindow::DrawInstancedShape(&mWindows, mWindowModelMatrices.size(), &WINDOW_VBO);
    glBindVertexArray(0);

    //PARTICLES
    for (int i = 0; i < SPAWN_PARTICLES; i++)
    {
        int index = GetUnusedParticleIndex(mParticles);
        mParticles[index].RespawnParticle();
    }

    float max = glm::distance(glm::vec3(26.1f, 0.0f, 26.1f), glm::vec3(26.0f, 0.0f, 26.0f));

    for (int i = 0; i < NUM_OF_PARTICLES; i++)
    {
        Particle& p = mParticles[i];

        float dist = glm::distance(p.mInfo.mPosition, glm::vec3(26.0f, 0.0f, 26.0f));
        p.mLife -=  (1.0f - (dist / max)) * DECAY_FACTOR;

        if (p.mLife > 0.0f)
        {
            p.mInfo.mPosition -= p.mVelocity * 0.1f * DECAY_FACTOR;
            p.mInfo.mAlpha -= (1.0f - (dist / max)) * DECAY_FACTOR;
        }
    }

    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindVertexArray(PARTICLE_VAO);
    OpenGLWindow::BindParticleBuffers(mParticles, &PARTICLE_VBO);
    OpenGLWindow::RenderParticles(mParticleShaderProgram);
    OpenGLWindow::SetTexture(mParticleShaderProgram, 5, "tex");
    OpenGLWindow::DrawParticles(mParticles, &PARTICLE_VBO);
    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_MULTISAMPLE);
}

//-----Render the scene using the shadow shader program.
//-----This draws the depth of the objects into a texture.
//-----Only objects that cast shadows need be rendered here.
void RenderSceneDepth(int room)
{
    //WALLS
    glBindVertexArray(HOUSE_VAO);
    OpenGLWindow::RenderShapeDepth(&mHouse, mShadowShaderProgram, room);
    glUniform1f(glGetUniformLocation(mShaderProgram, "far_plane"), 50.0f);
    OpenGLWindow::DrawShape(&mHouse, &HOUSE_VBO);
    glBindVertexArray(0);

    //DESK
    glBindVertexArray(DESK_VAO);
    OpenGLWindow::RenderShapeDepth(&mDesk, mShadowShaderProgram, room);
    glUniform1f(glGetUniformLocation(mShaderProgram, "far_plane"), 50.0f);
    OpenGLWindow::DrawShape(&mDesk, &DESK_VBO);
    glBindVertexArray(0);

    //DESK
    glBindVertexArray(TABLE_VAO);
    OpenGLWindow::RenderShapeDepth(&mTable , mShadowShaderProgram, room);
    glUniform1f(glGetUniformLocation(mShaderProgram, "far_plane"), 50.0f);
    OpenGLWindow::DrawShape(&mTable, &TABLE_VBO);
    glBindVertexArray(0);

    //RECLINER
    glBindVertexArray(RECLINER_VAO);
    OpenGLWindow::RenderShapeDepth(&mRecliner, mShadowShaderProgram, room);
    glUniform1f(glGetUniformLocation(mShaderProgram, "far_plane"), 50.0f);
    OpenGLWindow::DrawShape(&mRecliner, &RECLINER_VBO);
    glBindVertexArray(0);
}

//-----Generate the frame buffer objects for each light source
void GenerateFBO(int room)
{
    GLuint fbo;
    GLuint cubeMap;

    if (room == 1)
    {
        fbo = mDepthMapFBO;
        cubeMap = mDepthCubeMap;
        glActiveTexture(GL_TEXTURE7);
    }
    else if (room == 2)
    {
        fbo = mDepthMapFBO2;
        cubeMap = mDepthCubeMap2;
        glActiveTexture(GL_TEXTURE8);
    }
    else if (room == 3)
    {
        fbo = mDepthMapFBO3;
        cubeMap = mDepthCubeMap3;
        glActiveTexture(GL_TEXTURE9);
    }

    //Create an empty texture. The depth of the objects in the shadow pass will later be stored here
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

//-----Render the scene from the point of view of each light source.
//-----This stores the depth of the objects in a texture used later to determine shadows
void DepthPass(int room)
{
    GLuint fbo;
    glm::vec3 light;

    //Acquire the proper FBO based on the current room (ie: lightsource)
    if (room == 1)
    {
        fbo = mDepthMapFBO;
        light = lightPos;
    }
    else if (room == 2)
    {
        fbo = mDepthMapFBO2;
        light = lightPos2;
    }
    else if (room == 3)
    {
        fbo = mDepthMapFBO3;
        light = lightPos3;
    }

    //Prepare the 6 sides of the cube from the position of the light source
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
    glUseProgram(mShadowShaderProgram);
    // --- SENDING VP MATRIX TO GEOMETRY SHADER ----
    for (unsigned int i = 0; i < 6; ++i) {
        string n = "shadowMatrices[" + std::to_string(i) + "]";

        glUniformMatrix4fv(glGetUniformLocation(mShadowShaderProgram, n.c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
    }

    GLuint farPlaneLoc = glGetUniformLocation(mShadowShaderProgram, "far_plane");
    glUniform1f(farPlaneLoc, far_plane);

    RenderSceneDepth(room);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//-----The MAIN function, from here we start the application and run the game loop
int main()
{
    srand(0);
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

    mShaderProgram = OpenGLWindow::AttachShaders(vertexShader, fragmentShader);
    CHECK_ERROR(mShaderProgram);

    //--Skybox

    mSkyboxShaderProgram = OpenGLWindow::AttachShaders(vertexShaderSKYBOX, fragmentShaderSKYBOX);
    CHECK_ERROR(mSkyboxShaderProgram);

    //--Texture
    mTextureShaderProgram = OpenGLWindow::AttachShaders(vertexShader, textureFragmentShader);
    CHECK_ERROR(mTextureShaderProgram);

    //--Instanced
    mInstancedShaderProgram = OpenGLWindow::AttachShaders(vertexShaderInstanced, textureFragmentShader);
    CHECK_ERROR(mInstancedShaderProgram);

    //--Particle System
    mParticleShaderProgram = OpenGLWindow::AttachShaders(vertexShaderParticle, particleFragmentShader);
    CHECK_ERROR(mParticleShaderProgram);

    //--Shadows
    mShadowShaderProgram = OpenGLWindow::AttachShaders(vertexShaderShadows, fragmentShaderShadows, geometryShaderShadows);
    CHECK_ERROR(mParticleShaderProgram);

    OpenGLWindow::SetPointSize(7.5f);
    OpenGLWindow::SetCamera(&mCamera);

    //Create the objects in the scene

    //Shape object2 = ShapeGenerator::GenerateOBJ("burlap_sofa.obj", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -0.75f, 26.0f));
    mTable = ShapeGenerator::GenerateOBJ("kitchen_table.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(27.0616f, -0.75f, 32.7066f));
    //Shape object = ShapeGenerator::GenerateOBJ("kitchen_chair.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -0.5f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("shelves.obj", glm::vec3(0.0f), glm::vec3(0.1f), glm::vec3(0.0f), glm::vec3(26.0f, -0.75f, 26.0f));
    //Shape object2 = ShapeGenerator::GenerateOBJ("Recliner.obj", glm::vec3(0.0f), glm::vec3(0.75f), glm::vec3(0.0f), glm::vec3(30.0f, -0.5f, 26.0f));
    //cube = ShapeGenerator::GenerateOBJ("desk.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(33.0f, 0.0f, 28.0f));
    //cubeFloor = ShapeGenerator::GenerateOBJ("cube.obj", glm::vec3(0.0f), glm::vec3(20, 0.01, 10), glm::vec3(0.0f), glm::vec3(25.0f, -1.0f, 25.0f));

    mHouse = ShapeGenerator::GenerateOBJ("House.obj", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f, -90.0f, 180.0f), glm::vec3(30.6357574f, -1.0f, 30.0619736f));
    mRecliner = ShapeGenerator::GenerateOBJ("Recliner.obj", glm::vec3(0.0f), glm::vec3(0.75f), glm::vec3(0.0f), glm::vec3(30.0f, -0.5f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("bed.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -1.0f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("dresser.obj", glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(26.0f, -0.5f, 26.0f));
    mDesk = ShapeGenerator::GenerateOBJ("desk.obj", glm::vec3(0.0f), glm::vec3(0.1f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(26.0f, 0.0f, 26.0f));
    //Shape object2 = ShapeGenerator::GenerateOBJ("cheminee.obj", glm::vec3(1.0f), glm::vec3(0.15f), glm::vec3(0.0f), glm::vec3(30.0f, -0.5f, 26.0f));
    //Shape object = ShapeGenerator::GenerateOBJ("bed.obj", glm::vec3(1.0f), glm::vec3(0.075f, 0.05f, 0.125f), glm::vec3(0.0f), glm::vec3(30.0f, -0.5f, 26.0f));
    //Shape object2 = ShapeGenerator::GenerateOBJ("washing_machine.obj", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(30.0f, -0.5f, 26.0f));
    //Shape object2 = ShapeGenerator::GenerateOBJ("Blind.obj", glm::vec3(1.0f), glm::vec3(0.5f), glm::vec3(0.0f), glm::vec3(24.7f, 0.1f, 28.0f));
    //Shape object2 = ShapeGenerator::GenerateOBJ("lamp.obj", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(30.0f, 0.0f, 26.0f));
    //Shape object2 = ShapeGenerator::GenerateOBJ("light.obj", glm::vec3(1.0f), glm::vec3(0.3f), glm::vec3(0.0f), glm::vec3(28.0f, 0.7f, 28.0f));
    //Shape object2 = ShapeGenerator::GenerateOBJ("headboard.obj", glm::vec3(1.0f), glm::vec3(0.075f, 0.05f, 0.125f), glm::vec3(0.0f), glm::vec3(30.0f, -0.5f, 26.0f));

    mFloor = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f), -1.0f, false);
    mCeiling = ShapeGenerator::GenerateTerrain(glm::vec3(0.85f), 1.0f, true);

    ShapeGenerator::SetWallDimensions(1.0f, 2.0f, 0.25f);
    mWall = ShapeGenerator::GenerateWall();
    mTopWindow = ShapeGenerator::GenerateWindowWall();
    mBottomWindow = ShapeGenerator::GenerateWindowWall();
    mWindows = ShapeGenerator::GenerateWindow();

    //Skybox
    mSkybox = ShapeGenerator::GenerateCube(glm::vec3(1.0f), glm::vec3(100.0f));

    mCrosshair = ShapeGenerator::GenerateCrosshair();
    mMenu = ShapeGenerator::GenerateMenu();

    //Shape object3 = ShapeGenerator::GenerateOBJ("House.obj", glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f, -90.0f, 180.0f), glm::vec3(30.6357574f, -1.0f, 30.0619736f));

    for (int i = 0; i < NUM_OF_PARTICLES; i++)
        mParticles.push_back(Particle());

    mFloorModelMatrices = ShapeGenerator::GeneratePlaneModelMatrices(&mFloor);
    mCeilingModelMatrices = ShapeGenerator::GeneratePlaneModelMatrices(&mCeiling);

    //Prepare the buffer objects for each shape in the scene

    //----Skybox
    glGenVertexArrays(1, &SKYBOX_VAO);
    glGenBuffers(1, &SKYBOX_VBO);
    glGenTextures(1, &SKYBOX_TEXTURE);

    glBindVertexArray(SKYBOX_VAO);
    OpenGLWindow::BindBuffers(&mSkybox, &SKYBOX_VBO);

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

    OpenGLWindow::BindBuffers(&mHouse, &HOUSE_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Recliner
    glGenVertexArrays(1, &RECLINER_VAO);
    glGenBuffers(1, &RECLINER_VBO);

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &RECLINER_TEXTURE);

    glBindVertexArray(RECLINER_VAO);

    OpenGLWindow::BindBuffers(&mRecliner, &RECLINER_VBO);
    OpenGLWindow::BindTexture(&RECLINER_TEXTURE, "leather.jpg");
    OpenGLWindow::AddShape(&mRecliner);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Desk
    glGenVertexArrays(1, &DESK_VAO);
    glGenBuffers(1, &DESK_VBO);

    glActiveTexture(GL_TEXTURE6);
    glGenTextures(1, &DESK_TEXTURE);

    glBindVertexArray(DESK_VAO);

    OpenGLWindow::BindBuffers(&mDesk, &DESK_VBO);
    OpenGLWindow::BindTexture(&DESK_TEXTURE, "light_wood.jpg");
    OpenGLWindow::AddShape(&mDesk);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Table
    glGenVertexArrays(1, &TABLE_VAO);
    glGenBuffers(1, &TABLE_VBO);

    glActiveTexture(GL_TEXTURE6);
    glGenTextures(1, &TABLE_TEXTURE);

    glBindVertexArray(TABLE_VAO);

    OpenGLWindow::BindBuffers(&mTable, &TABLE_VBO);
    OpenGLWindow::BindTexture(&TABLE_TEXTURE, "light_wood.jpg");
    OpenGLWindow::AddShape(&mTable);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Floor
    glGenVertexArrays(1, &FLOOR_VAO);
    glGenBuffers(1, &FLOOR_VBO);
    glGenBuffers(1, &FLOOR_MODEL_MATRIX);

    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &FLOOR_TEXTURE);

    glBindVertexArray(FLOOR_VAO);

    OpenGLWindow::BindBuffers(&mFloor, &FLOOR_VBO);
    OpenGLWindow::BindModelBuffers(mFloorModelMatrices, &FLOOR_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&FLOOR_TEXTURE, "floor.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Ceiling
    glGenVertexArrays(1, &CEILING_VAO);
    glGenBuffers(1, &CEILING_VBO);
    glGenBuffers(1, &CEILING_MODEL_MATRIX);

    glActiveTexture(GL_TEXTURE3);
    glGenTextures(1, &CEILING_TEXTURE);

    glBindVertexArray(CEILING_VAO);

    OpenGLWindow::BindBuffers(&mCeiling, &CEILING_VBO);
    OpenGLWindow::BindModelBuffers(mCeilingModelMatrices, &CEILING_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&CEILING_TEXTURE, "ceiling.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Wall
    glGenVertexArrays(1, &WALL_VAO);
    glGenBuffers(1, &WALL_VBO);
    glGenBuffers(1, &WALL_MODEL_MATRIX);

    glActiveTexture(GL_TEXTURE4);
    glGenTextures(1, &WALL_TEXTURE);

    glBindVertexArray(WALL_VAO);

    mWallModelMatrices = ShapeGenerator::GenerateWallModelMatrices();
    OpenGLWindow::BindBuffers(&mWall, &WALL_VBO);
    OpenGLWindow::BindModelBuffers(mWallModelMatrices, &WALL_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&WALL_TEXTURE, "wallpaper.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Wall (top of window)
    glGenVertexArrays(1, &TOPWINDOW_VAO);
    glGenBuffers(1, &TOPWINDOW_VBO);
    glGenBuffers(1, &TOPWINDOW_MODEL_MATRIX);

    glBindVertexArray(TOPWINDOW_VAO);
    mTopWindowModelMatrices = ShapeGenerator::GetTopWindowMatrices();
    OpenGLWindow::BindBuffers(&mTopWindow, &TOPWINDOW_VBO);
    OpenGLWindow::BindModelBuffers(mTopWindowModelMatrices, &TOPWINDOW_MODEL_MATRIX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Wall (bottom of window)
    glGenVertexArrays(1, &BOTTOMWINDOW_VAO);
    glGenBuffers(1, &BOTTOMWINDOW_VBO);
    glGenBuffers(1, &BOTTOMWINDOW_MODEL_MATRIX);

    glBindVertexArray(BOTTOMWINDOW_VAO);

    mBottomWindowModelMatrices = ShapeGenerator::GetBottomWindowMatrices();
    OpenGLWindow::BindBuffers(&mBottomWindow, &BOTTOMWINDOW_VBO);
    OpenGLWindow::BindModelBuffers(mBottomWindowModelMatrices, &BOTTOMWINDOW_MODEL_MATRIX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Window
    glGenVertexArrays(1, &WINDOW_VAO);
    glGenBuffers(1, &WINDOW_VBO);
    glGenBuffers(1, &WINDOW_MODEL_MATRIX);
    glActiveTexture(GL_TEXTURE11);
    glGenTextures(1, &WINDOW_TEXTURE);

    glBindVertexArray(WINDOW_VAO);

    mWindowModelMatrices = ShapeGenerator::GetWindowMatrices();
    OpenGLWindow::BindBuffers(&mWindows, &WINDOW_VBO);
    OpenGLWindow::BindModelBuffers(mWindowModelMatrices, &WINDOW_MODEL_MATRIX);
    OpenGLWindow::BindTexture(&WINDOW_TEXTURE, "window.png");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Crosshair
    glGenVertexArrays(1, &CROSSHAIR_VAO);
    glGenBuffers(1, &CROSSHAIR_VBO);

    glBindVertexArray(CROSSHAIR_VAO);

    OpenGLWindow::BindBuffers(&mCrosshair, &CROSSHAIR_VBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Menu
    glActiveTexture(GL_TEXTURE10);
    glGenVertexArrays(1, &MENU_VAO);
    glGenBuffers(1, &MENU_VBO);
    glGenTextures(1, &MENU_TEXTURE);

    glBindVertexArray(MENU_VAO);

    OpenGLWindow::BindBuffers(&mMenu, &MENU_VBO);
    OpenGLWindow::BindTexture(&MENU_TEXTURE, "menu.png");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----Particles
    glActiveTexture(GL_TEXTURE5);
    glGenVertexArrays(1, &PARTICLE_VAO);
    glGenBuffers(1, &PARTICLE_VBO);
    glGenTextures(1, &PARTICLE_TEXTURE);

    glBindVertexArray(PARTICLE_VAO);

    OpenGLWindow::BindParticleBuffers(mParticles, &PARTICLE_VBO);
    OpenGLWindow::BindTexture(&PARTICLE_TEXTURE, "fire.jpg");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //----End

    //----Depth map frame buffer objects
    //Prepare the FBOs and cube maps for ecah light source
    glGenFramebuffers(1, &mDepthMapFBO);
    glActiveTexture(GL_TEXTURE7);
    glGenTextures(1, &mDepthCubeMap);
    GenerateFBO(1);

    glGenFramebuffers(1, &mDepthMapFBO2);
    glActiveTexture(GL_TEXTURE8);
    glGenTextures(1, &mDepthCubeMap2);
    GenerateFBO(2);

    glGenFramebuffers(1, &mDepthMapFBO3);
    glActiveTexture(GL_TEXTURE9);
    glGenTextures(1, &mDepthCubeMap3);
    GenerateFBO(3);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Set uniform values
    OpenGLWindow::SetUniformFactors(mShaderProgram);
    OpenGLWindow::SetUniformFactors(mTextureShaderProgram);
    OpenGLWindow::SetUniformFactors(mInstancedShaderProgram);

    bool mFirstPass = true;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        if (present)
            OpenGLWindow::JoystickCallback(window);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        OpenGLWindow::DefineViewport(window);

        if (mFirstPass || OpenGLWindow::mSelectedShapeIndex != -1)
        {
            mFirstPass = false;
            //--Depth pass (acquires shadows)
            DepthPass(1);
            DepthPass(2);
            DepthPass(3);
        }

        OpenGLWindow::DefineViewport(window);

        //--Render the scene normally using the textures obtained from the depth pass to create shadows
        RenderScene();

        //Render crosshair on top of everything
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(CROSSHAIR_VAO);
        OpenGLWindow::RenderHUDShape(&mCrosshair, mShaderProgram);
        OpenGLWindow::DrawLines(&mCrosshair, &CROSSHAIR_VBO);
        glBindVertexArray(0);

        //Display the menu
        if (OpenGLWindow::mMenuToggle)
        {
            glBindVertexArray(MENU_VAO);
            OpenGLWindow::RenderHUDShape(&mMenu, mTextureShaderProgram);
            OpenGLWindow::SetTexture(mTextureShaderProgram, 10, "textureSample");
            OpenGLWindow::DrawShape(&mMenu, &MENU_VBO);
            glBindVertexArray(0);
        }

        //Swap the screen buffers
        glfwSwapBuffers(window);
    }

    mRecliner.CleanUp();
    mDesk.CleanUp();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

