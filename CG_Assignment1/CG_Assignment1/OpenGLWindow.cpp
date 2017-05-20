#include "stdafx.h"
#include "OpenGLWindow.h"

GLRenderMode OpenGLWindow::mRenderMode = GL_TRIANGLES;
bool OpenGLWindow::mAnimationToggle = true;
bool OpenGLWindow::mAxisRotationToggle = false;
bool OpenGLWindow::mSinesoidToggle = false;
bool OpenGLWindow::mTeddyToggle = false;
int OpenGLWindow::mWaves = 3;
int OpenGLWindow::mWidth;
int OpenGLWindow::mHeight;
Camera* OpenGLWindow::mCamera;
glm::vec3 OpenGLWindow::mAxisOfRotation = glm::vec3(0.0f, 1.0f, 0.0f);
vector<Shape*> OpenGLWindow::mShapes;

void OpenGLWindow::InitializeGLFW()
{
    cout << "Starting GLFW context, OpenGL 3.3" << endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
}

GLenum OpenGLWindow::InitializeGLEW()
{
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    GLenum init = glewInit();
    if (init != GLEW_OK)
    {
        cerr << "Failed to initialize GLEW" << endl;
    }

    return init;
}

GLFWwindow* OpenGLWindow::CreateWindow()
{
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 1", nullptr, nullptr);
    if (window == nullptr)
    {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    return window;
}

void OpenGLWindow::DefineViewport(GLFWwindow* window)
{
    // Define the viewport dimensions
    glfwGetFramebufferSize(window, &mWidth, &mHeight);
    glViewport(0, 0, mWidth, mHeight);
}

GLfloat OpenGLWindow::GetAspectRatio()
{
    return (GLfloat) mWidth / (GLfloat) mHeight;
}

void OpenGLWindow::SetCamera(Camera* camera)
{
    if (mCamera)
    {
        cout << "Camera is already set!" << endl;
        return;
    }
    mCamera = camera;
}

string OpenGLWindow::CodeShader(string shaderPath)
{
    // Read the Shader code from the file
    string ShaderCode;
    ifstream ShaderStream(shaderPath, ios::in);

    if (ShaderStream.is_open()) {
        string Line = "";
        while (getline(ShaderStream, Line))
            ShaderCode += "\n" + Line;
        ShaderStream.close();
    }
    else {
        cerr << "Impossible to open " << shaderPath.c_str() << ". Are you in the right directory?\n" << endl;
        return "";
    }

    return ShaderCode;
}

GLuint OpenGLWindow::CompileShader(string shaderCode, int shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    char const * sourcePointer = shaderCode.c_str();
    glShaderSource(shader, 1, &sourcePointer, NULL);
    glCompileShader(shader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        string shaderString = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        cerr << "ERROR::SHADER::" << shaderString << "::COMPILATION_FAILED\n" << infoLog << endl;
        return -1;
    }

    return shader;
}

GLuint OpenGLWindow::AttachShaders(GLuint vertexShader, GLuint fragmentShader)
{
    GLint success;
    GLchar infoLog[512];
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
        return -1;
    }
    glDeleteShader(vertexShader); //free up memory
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    return shaderProgram;
}

void OpenGLWindow::AddShape(Shape* shape)
{
    mShapes.push_back(shape);
}

void OpenGLWindow::BindBuffers(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, shape->GetVertexBufferSize(), shape->mVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
}

void OpenGLWindow::RenderShape(Shape* shape, GLuint program)
{
    GLuint mvpLoc = glGetUniformLocation(program, "mvp_matrix");

    glm::mat4 model_matrix = glm::mat4(1.0f);

    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.y), mAxisOfRotation);
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::translate(model_matrix, shape->mTranslate);
    model_matrix = glm::scale(model_matrix, shape->mScale);

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = glm::perspective(45.0f, GetAspectRatio(), 0.01f, 100.0f);

    glm::mat4 mvp_matrix;
    mvp_matrix = projection_matrix * view_matrix * model_matrix;

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
}

void OpenGLWindow::DrawShape(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glDrawArrays(OpenGLWindow::mRenderMode, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// NOTE: ONLY FOR AXIS
void OpenGLWindow::DrawLines(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glDrawArrays(GL_LINE_STRIP, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWindow::CursorCallback(GLFWwindow* window, double x, double y)
{
    if (mCamera->mIsPanning)
        mCamera->Pan(glm::vec2(x, y));
    if (mCamera->mIsTilting)
        mCamera->Tilt(glm::vec2(x, y));
    if (mCamera->mIsZooming)
        mCamera->Zoom(glm::vec2(x, y));
}

void OpenGLWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        mAnimationToggle = !mAnimationToggle;

    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        mRenderMode = GL_LINES;

    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
        mRenderMode = GL_TRIANGLES;

    if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        mRenderMode = GL_POINTS;

    if (key == GLFW_KEY_UP && action == GLFW_REPEAT)
        if (!mAxisRotationToggle)
            mCamera->Rotate(X_AXIS, 1.0f);

    if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)
        if (!mAxisRotationToggle)
            mCamera->Rotate(X_AXIS, -1.0f);

    if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT)
    {
        if (!mAxisRotationToggle)
            mCamera->Rotate(Z_AXIS, 1.0f);
        else
        {
            mShapes[5]->mRotate.x += ROTATION_STEP; // Rotate axis
            glm::mat3 rotation = glm::mat3(glm::rotate(glm::radians(ROTATION_STEP), glm::vec3(1.0f, 0.0f, 0.0f)));
            mAxisOfRotation = rotation * mAxisOfRotation;
        }
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT)
    {
        if (!mAxisRotationToggle)
            mCamera->Rotate(Z_AXIS, -1.0f);
        else
        {
            mShapes[5]->mRotate.x -= ROTATION_STEP; // Rotate axis
            glm::mat3 rotation = glm::mat3(glm::rotate(glm::radians(-ROTATION_STEP), glm::vec3(1.0f, 0.0f, 0.0f)));
            mAxisOfRotation = rotation * mAxisOfRotation;
        }
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)
        {
            mShapes[0]->mScale += SCALE_STEP;
            mShapes[6]->mScale += SCALE_STEP;
        }
        else
        {
            mShapes[0]->mScale -= SCALE_STEP;
            mShapes[6]->mScale -= SCALE_STEP;
        }
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)
        {
            mShapes[1]->mScale += SCALE_STEP;
            mShapes[7]->mScale += SCALE_STEP;
        }
        else
        {
            mShapes[1]->mScale -= SCALE_STEP;
            mShapes[7]->mScale -= SCALE_STEP;
        }
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        mSinesoidToggle = !mSinesoidToggle;

    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
        mWaves++;

    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
        mWaves--;

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        mAxisRotationToggle = !mAxisRotationToggle;

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        mAxisOfRotation = glm::vec3(0.0f, 1.0f, 0.0f);
        mCamera->Reset();
        mShapes[5]->mRotate.x = 0.0f;
    }

    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        mTeddyToggle = !mTeddyToggle;
}

void OpenGLWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    mCamera->mIsPanning = button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS;
    mCamera->mIsTilting = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
    mCamera->mIsZooming = button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS;
}
