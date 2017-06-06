#include "stdafx.h"
#include "OpenGLWindow.h"

bool OpenGLWindow::mClose = false;
bool OpenGLWindow::mLight = true;
GLRenderMode OpenGLWindow::mRenderMode = GL_POINTS;
GLRenderMode OpenGLWindow::mSplineMode = GL_LINE_STRIP;
bool OpenGLWindow::mCurvatureToggle = false;
bool OpenGLWindow::mAnimationToggle = true;
bool OpenGLWindow::mTextureToggle = false;
bool OpenGLWindow::mSpawnToggle = true;
bool OpenGLWindow::mSplineToggle = false;
bool OpenGLWindow::mGenerateSpline = true;
float OpenGLWindow::mCurrentCameraIndex = 0.0f;
int OpenGLWindow::mIndex = 0;
int OpenGLWindow::mWidth;
int OpenGLWindow::mHeight;
int OpenGLWindow::mMaxPoints;
Camera* OpenGLWindow::mCamera;
vector<Shape*> OpenGLWindow::mShapes;
Shape* OpenGLWindow::mSplineShape;
Shape* OpenGLWindow::mSplineTrack;
GLuint* OpenGLWindow::mSplineVBO;
vector<glm::mat4> OpenGLWindow::mTrackModel;
GLuint* OpenGLWindow::mTrackModelVBO;

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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 2", nullptr, nullptr);
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

int OpenGLWindow::GetWidth()
{
    return mWidth;
}

int OpenGLWindow::GetHeight()
{
    return mHeight;
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

void OpenGLWindow::UpdateCamera()
{
    if (mCurrentCameraIndex >= mSplineTrack->mNumberOfVertices - 1)
        mCurrentCameraIndex = 0.0f;

    float x = mSplineTrack->mVertices[(int) mCurrentCameraIndex].mPosition.x;
    float z = mSplineTrack->mVertices[(int) mCurrentCameraIndex].mPosition.y;
    float next_x = mSplineTrack->mVertices[(int) mCurrentCameraIndex + 1].mPosition.x;
    float next_z = mSplineTrack->mVertices[(int) mCurrentCameraIndex + 1].mPosition.y;

    glm::vec3 current = glm::vec3((100.0f / mWidth)*x, 0.0f, (-100.0f / mHeight)*z);
    glm::vec3 next = glm::vec3((100.0f / mWidth)*next_x, 0.0f, (-100.0f / mHeight)*next_z);

    glm::vec3 position = glm::vec3((100.0f / mWidth)*x, 1.0f, (-100.0f / mHeight)*z);
    glm::vec3 direction = next - current;

    // Align camera based on trajection between current point and next point
    mCamera->Set(position, direction);

    if (mAnimationToggle)
    {
        if (!mCurvatureToggle)
            mCurrentCameraIndex += DISTANCE_CAMERA_SPEED;
        else
            mCurrentCameraIndex += CURVATURE_CAMERA_SPEED;
    }
}

void OpenGLWindow::SetMaxPoints(int number, GLFWwindow* window)
{
    mMaxPoints = number;
    glfwSetWindowShouldClose(window, GL_FALSE);
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

    return shaderProgram;
}

void OpenGLWindow::AttachTrackModel(GLuint* VBO)
{
    mTrackModelVBO = VBO;
}

void OpenGLWindow::AttachSpline(Shape* shape, GLuint* VBO)
{
    mSplineTrack = new Shape();
    mSplineShape = shape;
    mSplineVBO = VBO;
}

void OpenGLWindow::AddShape(Shape* shape)
{
    mShapes.push_back(shape);
}

void OpenGLWindow::BindBuffers(Shape* shape, GLuint* VBO, bool uv)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, shape->GetVertexBufferSize(), shape->mVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
    glEnableVertexAttribArray(2);
    if (uv)
    {
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 9));
        glEnableVertexAttribArray(3);
    }
}

void OpenGLWindow::BindOffsetBuffers(vector<glm::vec3>& offset, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);

    if (offset.size() > 0)
        glBufferData(GL_ARRAY_BUFFER, offset.size() * sizeof(glm::vec3), &offset[0], GL_STATIC_DRAW);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(4);
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

void OpenGLWindow::BindModelBuffers(vector<glm::mat4>& model, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);

    // Bind data
    if (model.size() > 0)
        glBufferData(GL_ARRAY_BUFFER, model.size() * sizeof(glm::mat4), &model[0], GL_STATIC_DRAW);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 4));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 8));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(6);

    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 12));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(7);
}

void OpenGLWindow::SetUniformFactors(GLuint program)
{
    glUseProgram(program);

    GLuint constantFactorLoc = glGetUniformLocation(program, "constantFactor");
    GLuint linearFactorLoc = glGetUniformLocation(program, "linearFactor");
    GLuint quadraticFactorLoc = glGetUniformLocation(program, "quadraticFactor");

    GLuint ambientLightLoc = glGetUniformLocation(program, "ambientLight");
    GLuint spotLightLoc = glGetUniformLocation(program, "spotLight");
    GLuint sunLightLoc = glGetUniformLocation(program, "sunLight");

    GLuint lightOnLoc = glGetUniformLocation(program, "lightOn");

    float constantFactor = CONSTANT_ATTENUATION;
    float linearFactor = LINEAR_ATTENUATION;
    float quadraticFactor = QUADRATIC_ATTENUATION;

    glm::vec3 ambient_light;
    glm::vec3 spot_light;
    glm::vec3 sun_light;

    ambient_light = glm::vec3(0.0f, 0.0f, 0.0f);
    spot_light = glm::vec3(0.0f, 10.0f, 0.0f);
    sun_light = glm::vec3(100.0f, 100.0f, 100.f);

    glUniform1f(constantFactorLoc, constantFactor);
    glUniform1f(linearFactorLoc, linearFactor);
    glUniform1f(quadraticFactorLoc, quadraticFactor);

    glUniform3fv(ambientLightLoc, 1, &ambient_light[0]);
    glUniform3fv(spotLightLoc, 1, &spot_light[0]);
    glUniform3fv(sunLightLoc, 1, &sun_light[0]);

    glUniform1i(lightOnLoc, mLight);
}

void OpenGLWindow::SetTexture(GLuint program, int index)
{
    GLuint textureLoc = glGetUniformLocation(program, "textureSample");
    GLuint textureOnLoc = glGetUniformLocation(program, "textureOn");

    glUniform1i(textureLoc, index);
    glUniform1i(textureOnLoc, mTextureToggle);
}

// TODO: only render with light on correct program
void OpenGLWindow::RenderShape(Shape* shape, GLuint program)
{
    glUseProgram(program);
    GLuint mvpLoc = glGetUniformLocation(program, "mvp_matrix");
    GLuint modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
    GLuint diffuseLightLoc = glGetUniformLocation(program, "diffuseLight");
    GLuint cameraPositionLoc = glGetUniformLocation(program, "cameraPosition");

    glm::mat4 model_matrix = glm::mat4(1.0f);

    model_matrix = glm::translate(model_matrix, shape->mTranslate);
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, shape->mScale);

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;

    if (mGenerateSpline)
        projection_matrix = glm::ortho(0.0f, (float)mWidth, (float)mHeight, 0.0f, 0.01f, 100.0f);
    else
        projection_matrix = glm::perspective(45.0f, GetAspectRatio(), 0.01f, 1000.0f);

    glm::mat4 mvp_matrix;
    mvp_matrix = projection_matrix * view_matrix * model_matrix;

    glm::vec3 diffuse_light;
    diffuse_light = mCamera->GetLightPosition();

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniform3fv(diffuseLightLoc, 1, &diffuse_light[0]);
    glUniform3fv(cameraPositionLoc, 1, &mCamera->GetPosition()[0]);
}

void OpenGLWindow::RenderTrack(Shape* shape, GLuint program)
{
    glUseProgram(program);
    GLuint vpLoc = glGetUniformLocation(program, "vp_matrix");
    GLuint diffuseLightLoc = glGetUniformLocation(program, "diffuseLight");
    GLuint cameraPositionLoc = glGetUniformLocation(program, "cameraPosition");

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = glm::perspective(45.0f, GetAspectRatio(), 0.01f, 1000.0f);

    glm::mat4 vp_matrix;
    vp_matrix = projection_matrix * view_matrix;

    glm::vec3 diffuse_light;
    diffuse_light = mCamera->GetLightPosition();

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp_matrix));
    glUniform3fv(diffuseLightLoc, 1, &diffuse_light[0]);
    glUniform3fv(cameraPositionLoc, 1, &mCamera->GetPosition()[0]);
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

void OpenGLWindow::DrawInstancedShape(Shape* shape, int size, GLuint* VAO, GLuint* VBO)
{
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 9));
    glPointSize(10.0f);
    glDrawArraysInstanced(mRenderMode, 0, shape->mNumberOfVertices, size);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glDrawArrays(mSplineMode, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWindow::Reset()
{
    mSplineToggle = false;
    mCurvatureToggle = false;
    mSplineShape = NULL;
    mSplineTrack = NULL;

    mIndex = 0;
    mSpawnToggle = true;

    mClose = false;
}

void OpenGLWindow::GenerateSpline()
{
    for (GLuint i = 0; i < mSplineShape->mNumberOfVertices - 1; i++)
    {
        glm::mat4 model_matrix = glm::mat4(1.0f);

        float x = mSplineShape->mVertices[i].mPosition.x;
        float z = mSplineShape->mVertices[i].mPosition.y;

        model_matrix = glm::translate(model_matrix, glm::vec3((100.0f / mWidth)*x, 0.0f, (-100.0f / mHeight)*z));

        float next_x = mSplineShape->mVertices[i + 1].mPosition.x;
        float next_z = mSplineShape->mVertices[i + 1].mPosition.y;

        glm::vec3 v = glm::normalize(glm::vec3(next_x, 0.0f, next_z) - glm::vec3(x, 0.0f, z));
        float angle = acos(glm::dot(v, glm::vec3(0.0f, 0.0f, 1.0f)));

        if (v.x > 0)
            angle = -angle;

        model_matrix = glm::rotate(model_matrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.0f));

        mTrackModel.push_back(model_matrix);
    }

    BindModelBuffers(mTrackModel, mTrackModelVBO);
}

void OpenGLWindow::CursorCallback(GLFWwindow* window, double x, double y)
{
}

void OpenGLWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        mClose = true;
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        if (mGenerateSpline && !mSplineToggle)
        {
            mSpawnToggle = false;

            vector<glm::vec3> vertices;
            for (int i = 0; i < mIndex; i++)
            {
                SKIP(i);
                vertices.push_back(mShapes[i]->mTranslate);
            }

            // Generate spline for display and for traversing
            if (mCurvatureToggle) // Generate based on curvature
            {
                *mSplineShape = ShapeGenerator::GenerateCatmullRomSpline(vertices, 175.0f);
                *mSplineTrack = ShapeGenerator::GenerateCatmullRomSpline(vertices, 178.0f);
            }
            else // Generature based on distance
            {
                *mSplineShape = ShapeGenerator::GenerateCatmullRomSpline(vertices, 100.0f);
                *mSplineTrack = ShapeGenerator::GenerateCatmullRomSpline(vertices, 5.0f);
            }
            BindBuffers(mSplineShape, mSplineVBO, false);
            AddShape(mSplineShape);

            mSplineToggle = true;
        }
        else if (!mGenerateSpline)
        {
            mLight = !mLight;
        }
    }

    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
    {
        if (mGenerateSpline)
        {
            // Restart spline drawing
            Reset();
            mShapes.clear();
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        else
        {
            // Restart Camera at beginning of spline
            mCurrentCameraIndex = 0.0f;
        }
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        if (mGenerateSpline)
        {
            // Switch to curvature
            mCurvatureToggle = !mCurvatureToggle;

            delete(mSplineShape->mVertices);
            mSplineShape->mNumberOfVertices = 0;
            delete(mSplineTrack->mVertices);
            mSplineTrack->mNumberOfVertices = 0;

            vector<glm::vec3> vertices;
            for (int i = 0; i < mIndex; i++)
            {
                SKIP(i);
                vertices.push_back(mShapes[i]->mTranslate);
            }

            if (mCurvatureToggle) // Generate based on curvature
            {
                *mSplineShape = ShapeGenerator::GenerateCatmullRomSpline(vertices, 175.0f);
                *mSplineTrack = ShapeGenerator::GenerateCatmullRomSpline(vertices, 178.0f);
            }
            else // Generature based on distance
            {
                *mSplineShape = ShapeGenerator::GenerateCatmullRomSpline(vertices, 100.0f);
                *mSplineTrack = ShapeGenerator::GenerateCatmullRomSpline(vertices, 5.0f);
            }

            BindBuffers(mSplineShape, mSplineVBO, false);
            AddShape(mSplineShape);
        }
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        mSplineMode = GL_LINE_STRIP;

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        mSplineMode = GL_POINTS;

    if (key == GLFW_KEY_T && action == GLFW_PRESS) // Toggle textures
        mTextureToggle = !mTextureToggle;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && mSplineToggle)
    {
        if (mGenerateSpline)
        {
            // Switch to 3D environment
            mGenerateSpline = false;
            mRenderMode = GL_TRIANGLES;

            GenerateSpline();
        }
        else
        {
            // Toggle animation
            mAnimationToggle = !mAnimationToggle;
        }
    }
}

void OpenGLWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (mIndex < mMaxPoints + 1 && mSpawnToggle)
        {
            SKIP(mIndex);

            double x, y;
            glfwGetCursorPos(window, &x, &y);

            // Add point to Shapes vector
            mShapes[mIndex]->mTranslate = glm::vec3((float)x, (float)y, 0.0f);

            mIndex++;
        }
    }
}
