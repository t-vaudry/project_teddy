#include "stdafx.h"
#include "OpenGLWindow.h"

// Public static variables
GLRenderMode OpenGLWindow::mRenderMode = GL_TRIANGLES;
int OpenGLWindow::mWidth;
int OpenGLWindow::mHeight;
Camera* OpenGLWindow::mCamera;
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
    return (GLfloat)mWidth / (GLfloat)mHeight;
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 9));
    glEnableVertexAttribArray(3);
}

void OpenGLWindow::BindTexture(GLuint* texture, char* path)
{
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
}

void OpenGLWindow::SetTexture(GLuint program, int index)
{
    GLuint textureLoc = glGetUniformLocation(program, "textureSample");
    glUniform1i(textureLoc, index);
}

void OpenGLWindow::RenderShape(Shape* shape, GLuint program)
{
    GLuint mvpLoc = glGetUniformLocation(program, "mvp_matrix");

    glm::mat4 model_matrix = glm::mat4(1.0f);

    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
    glPointSize(10.0f);
    glDrawArrays(mRenderMode, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// NOTE: ONLY FOR AXIS
void OpenGLWindow::DrawLines(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
    glPointSize(5.0f);
    glDrawArrays(GL_LINE_STRIP, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::vec3 OpenGLWindow::GetNoCollisionPosition(glm::vec3 startPos, glm::vec3 desiredEndPos)
{
    //Loop through all shapes in world
    //If within BV of shape, return contact point
    //Else, return desiredEndPoint
    glm::vec3 returnPos = desiredEndPos;
    for (int i = 0; i < mShapes.size(); i++)
    {
        //Get distance from desiredEndPos to center of sphere (Do not use centre of miniball, but rather translation)
        glm::vec3 centreOnPlane = mShapes[i]->mTranslate;
        float distanceToCentre = glm::length(desiredEndPos - centreOnPlane);

        //If the distance is less than the radius, we are inside, so return the contact point
        if (distanceToCentre < mShapes[i]->mRadius)
        {
            glm::vec3 l = glm::normalize(desiredEndPos - startPos);

            float d1 = -(glm::dot(l, startPos - centreOnPlane)) + sqrt(pow(glm::dot(l, startPos - centreOnPlane), 2.0f) - pow(glm::length(startPos - centreOnPlane), 2.0f) + pow(mShapes[i]->mRadius, 2.0f));
            float d2 = -(glm::dot(l, startPos - centreOnPlane)) - sqrt(pow(glm::dot(l, startPos - centreOnPlane), 2.0f) - pow(glm::length(startPos - centreOnPlane), 2.0f) + pow(mShapes[i]->mRadius, 2.0f));
            returnPos = glm::min(d1, d2) * l + startPos;
            break;
        }
    }

    return returnPos;
}

void OpenGLWindow::CursorCallback(GLFWwindow* window, double x, double y)
{
    mCamera->SetLookAt(glm::vec2(x, y));
}

void OpenGLWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //DEBUG
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        mRenderMode = GL_LINE_STRIP;

    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() - CAMERA_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f);

        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos));
    }

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() + CAMERA_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f);

        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos));
    }

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() + CAMERA_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f);

        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos));
    }

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() - CAMERA_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f);

        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos));
    }
}

void OpenGLWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}
