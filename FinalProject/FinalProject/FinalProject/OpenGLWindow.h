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

#include "Camera.h"
#include "Shape.h"
#include "ShapeGenerator.h"

#define WIDTH 2000
#define HEIGHT 2000
#define SCALE_STEP 0.1f
#define ROTATION_STEP 0.5f
#define MOVE_STEP 0.25f

typedef int GLRenderMode;

using namespace std;

class OpenGLWindow
{
private:
    static int mWidth;
    static int mHeight;
    static Camera* mCamera;
    static vector<Shape*> mShapes;
public:
    static GLRenderMode mRenderMode;

    static void CursorCallback(GLFWwindow* window, double x, double y);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);

    static void InitializeGLFW();
    static GLenum InitializeGLEW();
    static GLFWwindow* CreateWindow();
    static void DefineViewport(GLFWwindow* window);
    static GLfloat GetAspectRatio();
    static void SetCamera(Camera* camera);
    static inline void SetPointSize(GLfloat size) { glPointSize(size); }
    static string CodeShader(string shaderPath);
    static GLuint CompileShader(string shaderCode, int shaderType);
    static GLuint AttachShaders(GLuint vertexShader, GLuint fragmentShader);
    static void AddShape(Shape* shape);
    static void BindBuffers(Shape* shape, GLuint* VBO);
    static void BindTexture(GLuint* texture, char* path);
    static void SetTexture(GLuint program, int index);
    static void RenderShape(Shape* shape, GLuint program);
    static void DrawShape(Shape* shape, GLuint* VBO);
    static void DrawLines(Shape* shape, GLuint* VBO);
};

