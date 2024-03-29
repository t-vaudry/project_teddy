#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <glew.h>   // include GL Extension Wrangler
#include <glfw3.h>  // include GLFW helper library
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include <SOIL.h>

#include <Camera.h>
#include <Shape.h>
#include <ShapeGenerator.h>
#include <Particle.h>

#define WIDTH 2000
#define HEIGHT 2000
#define NEAR_PLANE 0.01f
#define FAR_PLANE 1000.0f
#define BIAS 0.25f
#define CAMERA_MOVEMENT_SPEED 0.05f
#define SHAPE_MOVEMENT_SPEED 0.075f
#define ROTATION_SPEED 2.5f

#define CONSTANT_ATTENUATION 0.75f
#define LINEAR_ATTENUATION 0.15f
#define QUADRATIC_ATTENUATION 0.01f

typedef int GLRenderMode;

using namespace std;

class OpenGLWindow
{
private:
    static Camera* mCamera;
    static bool mOrient;
    static GLuint mPrevStartButtonState;
    static GLuint mPrevButtonXState;
    static GLuint mPrevButtonAState;
    static GLuint mPrevButtonYState;
    static glm::vec3 mLightSwitch;
    static glm::vec3 mLightIntensity;
    static bool mToggleLight;

    //For bed
    static bool mBedSelected;
public:
    static vector<Shape*> mShapes;
    static bool mDebug;
    static int mWidth;
    static int mHeight;
    static GLRenderMode mRenderMode;
    static bool mMenuToggle;
    static int mSelectedShapeIndex;
    static float mLightHeight;
    static glm::vec3 mRoom1Light;
    static glm::vec3 mRoom2Light;
    static glm::vec3 mRoom3Light;

    static void CursorCallback(GLFWwindow* window, double x, double y);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
    static void JoystickCallback(GLFWwindow* window);

    static void InitializeGLFW();
    static GLenum InitializeGLEW();
    static GLFWwindow* CreateWindow();
    static void DefineViewport(GLFWwindow* window);

    static GLfloat GetAspectRatio();
    static glm::mat4 GetProjectionMatrix();
    static int GetCurrentRoom(glm::vec3 pos);
    static glm::vec3 GetNoCollisionPosition(glm::vec3 startPos, glm::vec3 desiredEndPos, int ignoreIndex = -1);
    static bool GetIsValidObjectPosition(int objectIndex);

    static void SetCamera(Camera* camera);
    static inline void SetPointSize(GLfloat size) { glPointSize(size); }
    static inline void SetLineWidth(GLfloat size) { glLineWidth(size); }

    static string CodeShader(string shaderPath);
    static GLuint CompileShader(string shaderCode, int shaderType);
    static GLuint AttachShaders(GLuint vertexShader, GLuint fragmentShader);
    static GLuint AttachShaders(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader);
    static void AddShape(Shape* shape);

    static void BindBuffers(Shape* shape, GLuint* VBO);
    static void BindSkyboxTexture(GLuint* texture, vector<const GLchar*> faces);
    static void BindTexture(GLuint* texture, char* path);
    static void BindModelBuffers(vector<glm::mat4>& model, GLuint* VBO);
    static void BindParticleBuffers(vector<Particle>& particles, GLuint* VBO);

    static void SetUniformFactors(GLuint program);
    static void SetTexture(GLuint program, int index, char* name);
    static void SetLights();

    static void RenderAABB(Shape* shape, GLuint program);
    static void RenderShape(Shape* shape, GLuint program);
    static void RenderShapeDepth(Shape* shape, GLuint program, int room);
    static void RenderHUDShape(Shape* shape, GLuint program);
    static void RenderInstancedShape(Shape* shape, GLuint program);
    static void RenderParticles(GLuint program);

    static void DrawPoint(Shape* shape, GLuint* VBO);
    static void DrawShape(Shape* shape, GLuint* VBO);
    static void DrawInstancedShape(Shape* shape, int size, GLuint* VBO);
    static void DrawSkybox(Shape* shape, GLuint* VBO);
    static void DrawLines(Shape* shape, GLuint* VBO);
    static void DrawParticles(vector<Particle>& particles, GLuint* VBO);
};

