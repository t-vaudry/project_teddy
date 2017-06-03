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

#define WIDTH 1800
#define HEIGHT 1800
#define DISTANCE_CAMERA_SPEED 0.25f
#define CURVATURE_CAMERA_SPEED 0.35f;

#define CONSTANT_ATTENUATION 1.0f
#define LINEAR_ATTENUATION 1.0f
#define QUADRATIC_ATTENUATION 0.5f

#define SKIP(a) if (a == 1) a++;

typedef int GLRenderMode;

using namespace std;

class OpenGLWindow
{
private:
    static int mWidth;
    static int mHeight;
    static Camera* mCamera;
    static int mMaxPoints;
    static float mCurrentCameraIndex;
public:
    static bool mClose;
    static bool mLight;
    static GLRenderMode mRenderMode;
    static bool mCurvatureToggle;
    static bool mGenerateSpline;
    static GLRenderMode mSplineMode;
    static bool mAnimationToggle;
    static bool mTextureToggle;
    static bool mSpawnToggle;
    static bool mSplineToggle;
    static Shape* mSplineShape;
    static Shape* mSplineTrack;
    static GLuint* mSplineVBO;
    static vector<glm::mat4> mTrackModel;
    static GLuint* mTrackModelVBO;
    static vector<Shape*> mShapes;
    static int mIndex;

    static void CursorCallback(GLFWwindow* window, double x, double y);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);

    static void InitializeGLFW();
    static GLenum InitializeGLEW();
    static GLFWwindow* CreateWindow();
    static void DefineViewport(GLFWwindow* window);
    static GLfloat GetAspectRatio();
    static int GetWidth();
    static int GetHeight();
    static void SetCamera(Camera* camera);
    static void UpdateCamera();
    static void SetMaxPoints(int number, GLFWwindow* window);
    static inline void SetPointSize(GLfloat size) { glPointSize(size); }
    static string CodeShader(string shaderPath);
    static GLuint CompileShader(string shaderCode, int shaderType);
    static GLuint AttachShaders(GLuint vertexShader, GLuint fragmentShader);
    static void AttachTrackModel(GLuint* VBO);
    static void AttachSpline(Shape* shape, GLuint* VBO);
    static void AddShape(Shape* shape);
    static void BindBuffers(Shape* shape, GLuint* VBO, bool uv);
    static void BindOffsetBuffers(vector<glm::vec3>& offset, GLuint* VBO);
    static void BindTexture(GLuint* texture, char* path);
    static void BindModelBuffers(vector<glm::mat4>& model, GLuint* VBO);
    static void SetUniformFactors(GLuint program);
    static void SetTexture(GLuint program, int index);
    static void RenderShape(Shape* shape, GLuint program);
    static void RenderTrack(Shape* shape, GLuint program);
    static void DrawShape(Shape* shape, GLuint* VBO);
    static void DrawInstancedShape(Shape* shape, int size, GLuint* VAO, GLuint* VBO);
    static void DrawLines(Shape* shape, GLuint* VBO);
    static void Reset();
    static void GenerateSpline();
};