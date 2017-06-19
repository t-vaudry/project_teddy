#include <stdafx.h>
#include <OpenGLWindow.h>

// Public static variables
GLRenderMode OpenGLWindow::mRenderMode = GL_TRIANGLES;
int OpenGLWindow::mWidth;
int OpenGLWindow::mHeight;
Camera* OpenGLWindow::mCamera;
bool OpenGLWindow::mOrient = false;
vector<Shape*> OpenGLWindow::mShapes;
bool OpenGLWindow::mDebug = false;
bool OpenGLWindow::mMenuToggle = false;
GLuint OpenGLWindow::mPrevStartButtonState = 0;
GLuint OpenGLWindow::mPrevButtonAState = 0;
GLuint OpenGLWindow::mPrevButtonYState = 0;
glm::vec3 OpenGLWindow::mLightSwitch = glm::vec3(1.0f);
bool OpenGLWindow::mToggleLight = true;

int OpenGLWindow::mSelectedShapeIndex = -1;
glm::vec3 OpenGLWindow::mSunLight = glm::vec3(28.0f, 0.0f, 28.0f);

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
    //GLFWwindow* window = glfwCreateWindow(3840, 2160, "Project", glfwGetPrimaryMonitor(), nullptr);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Project", nullptr, nullptr);
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
        string shaderString;
        switch (shaderType)
        {
        case GL_VERTEX_SHADER:
            shaderString = "VERTEX";
            break;
        case GL_FRAGMENT_SHADER:
            shaderString = "FRAGMENT";
            break;
        };
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

GLuint OpenGLWindow::AttachShaders(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
{
    GLint success;
    GLchar infoLog[512];
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geometryShader);
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
    glDeleteShader(geometryShader);

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

void OpenGLWindow::BindSkyboxTexture(GLuint* texture, vector<const GLchar*> faces)
{
    int width, height;
    unsigned char* image = nullptr;
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture);
    for (GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
            );
        SOIL_free_image_data(image); //free resources
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

void OpenGLWindow::BindParticleBuffers(vector<Particle>& particles, GLuint* VBO)
{
    vector<ParticleInfo> info;
    for (unsigned int i = 0; i < particles.size(); i++)
        info.push_back(particles[i].mInfo);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, info.size() * sizeof(ParticleInfo), &info[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 8));
    glEnableVertexAttribArray(3);
}

void OpenGLWindow::SetUniformFactors(GLuint program)
{
    glUseProgram(program);

    GLuint constantFactorLoc = glGetUniformLocation(program, "constantFactor");
    GLuint linearFactorLoc = glGetUniformLocation(program, "linearFactor");
    GLuint quadraticFactorLoc = glGetUniformLocation(program, "quadraticFactor");

    GLuint ambientLightLoc = glGetUniformLocation(program, "ambientLight");
    GLuint sunLightLoc = glGetUniformLocation(program, "sunLight");
    GLuint farPlaneLoc = glGetUniformLocation(program, "farPlane");

    float constantFactor = CONSTANT_ATTENUATION;
    float linearFactor = LINEAR_ATTENUATION;
    float quadraticFactor = QUADRATIC_ATTENUATION;

    glm::vec3 ambient_light;
    glm::vec3 sun_light;

    ambient_light = glm::vec3(0.25f);
    sun_light = mSunLight;

    glUniform1f(constantFactorLoc, constantFactor);
    glUniform1f(linearFactorLoc, linearFactor);
    glUniform1f(quadraticFactorLoc, quadraticFactor);

    glUniform3fv(ambientLightLoc, 1, &ambient_light[0]);
    glUniform3fv(sunLightLoc, 1, &sun_light[0]);
    glUniform1f(farPlaneLoc, FAR_PLANE);
}

void OpenGLWindow::SetTexture(GLuint program, int index, char* name)
{
    GLuint textureLoc = glGetUniformLocation(program, name);
    glUniform1i(textureLoc, index);
}

void OpenGLWindow::RenderShape(Shape* shape, GLuint program)
{
    glUseProgram(program);
    GLuint mvpLoc = glGetUniformLocation(program, "mvp_matrix");
    GLuint modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
    GLuint alphaLoc = glGetUniformLocation(program, "alpha");
    GLuint invalidPosLoc = glGetUniformLocation(program, "invalidPosition");
    GLuint lightSwitchLoc = glGetUniformLocation(program, "lightSwitch");
    GLuint noLightLoc = glGetUniformLocation(program, "noLight");

    glm::mat4 model_matrix = glm::mat4(1.0f);

    model_matrix = glm::translate(model_matrix, shape->mTranslate);
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, shape->mScale);


    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = GetProjectionMatrix();

    glm::mat4 mvp_matrix;
    mvp_matrix = projection_matrix * view_matrix * model_matrix;

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniform1f(alphaLoc, shape->mAlpha);
    glUniform1i(invalidPosLoc, !shape->mValidPos);
    glUniform3fv(lightSwitchLoc, 1, &mLightSwitch[0]);
    glUniform1f(noLightLoc, 0.0f);
}

void OpenGLWindow::RenderShapeDepth(Shape* shape, GLuint program, int room)
{
    glUseProgram(program);
    GLuint projectionLoc = glGetUniformLocation(program, "projection_matrix");
    GLuint viewMatrixLoc = glGetUniformLocation(program, "view_matrix");
    GLuint transformLoc = glGetUniformLocation(program, "model_matrix");
    GLuint lightPosLoc = glGetUniformLocation(program, "light_position");

    glm::mat4 model_matrix = glm::mat4(1.0f);

    model_matrix = glm::translate(model_matrix, shape->mTranslate);
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.y), glm::vec3(0.0f, 15.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, shape->mScale);

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = GetProjectionMatrix();

    glm::vec3 lightPos;

    if (room == 1)
        lightPos = glm::vec3(28.0f, 0.0f, 28.0f);
    else if (room == 2)
        lightPos = glm::vec3(33.0f, 0.0f, 28.0f);
    else
        lightPos= glm::vec3(28.0f, 0.0f, 33.0f);


    glUniform3fv(lightPosLoc, 1, &lightPos[0]);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
}

void OpenGLWindow::RenderHUDShape(Shape* shape, GLuint program)
{
    glUseProgram(program);
    GLuint mvpLoc = glGetUniformLocation(program, "mvp_matrix");
    GLuint modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
    GLuint alphaLoc = glGetUniformLocation(program, "alpha");
    GLuint noLightLoc = glGetUniformLocation(program, "noLight");

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(mWidth / 2.0f, mHeight / 2.0f, 0.0f));

    glm::mat4 projection_matrix;
    projection_matrix = glm::ortho(0.0f, (float)mWidth, (float)mHeight, 0.0f, -1.0f, 1.0f);

    glm::mat4 mvp_matrix;
    mvp_matrix = projection_matrix * model_matrix;

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniform1f(alphaLoc, 1.0f);
    glUniform1f(noLightLoc, 1.0f);
}

void OpenGLWindow::RenderInstancedShape(Shape* shape, GLuint program)
{
    glUseProgram(program);
    GLuint vpLoc = glGetUniformLocation(program, "vp_matrix");
    GLuint alphaLoc = glGetUniformLocation(program, "alpha");
    GLuint lightSwitchLoc = glGetUniformLocation(program, "lightSwitch");
    GLuint noLightLoc = glGetUniformLocation(program, "noLight");

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = GetProjectionMatrix();

    glm::mat4 vp_matrix;
    vp_matrix = projection_matrix * view_matrix;

    //glm::vec3 light_position = glm::vec3(28.0f, 0.0f, 35.0f);

    //glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
    //glUniform3fv(lightPosLoc, 1, &light_position[0]);

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp_matrix));
    glUniform1f(alphaLoc, shape->mAlpha);
    glUniform3fv(lightSwitchLoc, 1, &mLightSwitch[0]);
    glUniform1f(noLightLoc, 0.0f);
}

void OpenGLWindow::RenderParticles(GLuint program)
{
    glUseProgram(program);
    GLuint vpLoc = glGetUniformLocation(program, "vp_matrix");

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = GetProjectionMatrix();

    glm::mat4 vp_matrix;
    vp_matrix = projection_matrix * view_matrix;

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp_matrix));
}

void OpenGLWindow::DrawPoint(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glDrawArrays(GL_POINTS, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWindow::DrawShape(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glDrawArrays(mRenderMode, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWindow::DrawInstancedShape(Shape* shape, int size, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glDrawArraysInstanced(mRenderMode, 0, shape->mNumberOfVertices, size);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWindow::DrawSkybox(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
    glDrawArrays(mRenderMode, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// NOTE: ONLY FOR AXIS
void OpenGLWindow::DrawLines(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glLineWidth(10.0f);
    glDrawArrays(GL_LINES, 0, shape->mNumberOfVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLWindow::DrawParticles(vector<Particle>& particles, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::mat4 OpenGLWindow::GetProjectionMatrix()
{
    return glm::perspective(45.0f, GetAspectRatio(), NEAR_PLANE, FAR_PLANE);
}

int OpenGLWindow::GetCurrentRoom(glm::vec3 pos)
{
    //Determine which room we are currently in
    float room1Xmin = 24.70f;
    float room1Xmax = 30.25f;
    float room1Zmin = 25.15f;
    float room1Zmax = 29.85f;

    float room2Xmin = 30.65f;
    float room2Xmax = 35.35f;
    float room2Zmin = 25.15f;
    float room2Zmax = 29.85f;

    float room3Xmin = 24.70f;
    float room3Xmax = 30.25f;
    float room3Zmin = 30.25f;
    float room3Zmax = 34.75f;

    float door12Xmin = 29.99f;
    float door12Xmax = 30.75f;
    float door12Zmin = 27.00f;
    float door12Zmax = 28.00f;

    float door23Xmin = 26.50f;
    float door23Xmax = 27.50f;
    float door23Zmin = 29.74f;
    float door23Zmax = 30.26f;

    int room = -1;
    if (pos.x > room1Xmin && pos.x < room1Xmax && pos.z > room1Zmin && pos.z < room1Zmax)
    {
        room = 1;
    }
    else if (pos.x > room2Xmin && pos.x < room2Xmax && pos.z > room2Zmin && pos.z < room2Zmax)
    {
        room = 2;
    }
    else if (pos.x > room3Xmin && pos.x < room3Xmax && pos.z > room3Zmin && pos.z < room3Zmax)
    {
        room = 3;
    }
    else if (pos.x > door12Xmin && pos.x < door12Xmax && pos.z > door12Zmin && pos.z < door12Zmax)
    {
        room = 4;
    }
    else if (pos.x > door23Xmin && pos.x < door23Xmax && pos.z > door23Zmin && pos.z < door23Zmax)
    {
        room = 5;
    }

    return room;
}

glm::vec3 OpenGLWindow::GetNoCollisionPosition(glm::vec3 startPos, glm::vec3 desiredEndPos, bool& valid, int ignoreIndex)
{
    //Determine the current room
    //cout << GetCurrentRoom(startPos) << endl;
    //Loop through all shapes in world
    //If within BV of shape, return contact point
    //Else, return desiredEndPoint
    glm::vec3 returnPos = desiredEndPos;

    for (unsigned int i = 0; i < mShapes.size(); i++)
    {
        if (i == ignoreIndex)
            continue;

        BoundingBox camera = BoundingBox(desiredEndPos, desiredEndPos);
        if (camera.Intersect(mShapes[i]->mBox) && mShapes[i]->mTranslate.y < 0.5f)
        {
            returnPos = startPos;
        }
    }

    if (GetCurrentRoom(returnPos) == -1)
    {
        valid = false;
        returnPos = startPos;
    }

    return returnPos;
}

bool OpenGLWindow::GetIsValidObjectPosition(int objectIndex)
{
    bool valid = true;

    glm::vec3 centrePos = mShapes[objectIndex]->mCenter;

    glm::vec3 xPos = glm::vec3(mShapes[objectIndex]->mBox.mMax.x, 0.0f, mShapes[objectIndex]->mBox.mMax.z);
    glm::vec3 zPos = glm::vec3(mShapes[objectIndex]->mBox.mMin.x, 0.0f, mShapes[objectIndex]->mBox.mMax.z);

    glm::vec3 minusXPos = glm::vec3(mShapes[objectIndex]->mBox.mMax.x, 0.0f, mShapes[objectIndex]->mBox.mMin.z);
    glm::vec3 minusZPos = glm::vec3(mShapes[objectIndex]->mBox.mMin.x, 0.0f, mShapes[objectIndex]->mBox.mMin.z);

    //Check if outside
    if (GetCurrentRoom(centrePos) == -1 || GetCurrentRoom(xPos) == -1 || GetCurrentRoom(zPos) == -1 || GetCurrentRoom(minusXPos) == -1 || GetCurrentRoom(minusZPos) == -1)
    {
        return false;
    }

    for (unsigned int i = 0; i < mShapes.size(); i++)
    {
        if (i == objectIndex)
            continue;

        //If the distance is less than the radius, we are inside, so return the contact point
        if (mShapes[objectIndex]->mBox.Intersect(mShapes[i]->mBox))
        {
            return false;
        }
    }

    return true;
}

void OpenGLWindow::CursorCallback(GLFWwindow* window, double x, double y)
{
    if (mOrient)
        mCamera->SetLookAt(glm::vec2(x, y));
}

void OpenGLWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //DEBUG
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        mDebug = !mDebug;

    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        if (mRenderMode == GL_TRIANGLES)
            mRenderMode = GL_LINE_STRIP;
        else
            mRenderMode = GL_TRIANGLES;
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        if (mode == GLFW_MOD_SHIFT)
        {
            mToggleLight = !mToggleLight;
            if (mToggleLight)
                mLightSwitch = glm::vec3(1.0f);
            else
                mLightSwitch = glm::vec3(0.0f);
        }
        else
        {
            int room = GetCurrentRoom(mCamera->GetPosition());
            if (mLightSwitch[room - 1])
                mLightSwitch[room - 1] = 0.0f;
            else
                mLightSwitch[room - 1] = 1.0f;
        }
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() - CAMERA_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));

        //cout << mCamera->GetPosition().x << ", " << mCamera->GetPosition().z << endl;
    }

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() + CAMERA_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));

        //cout << mCamera->GetPosition().x << ", " << mCamera->GetPosition().z << endl;
    }

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() + CAMERA_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));

        //cout << mCamera->GetPosition().x << ", " << mCamera->GetPosition().z << endl;
    }

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        glm::vec3 desiredPos = mCamera->GetPosition() - CAMERA_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));

        //cout << mCamera->GetPosition().x << ", " << mCamera->GetPosition().z << endl;
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && mSelectedShapeIndex != -1)
    {
        bool valid = true;

        valid = GetIsValidObjectPosition(mSelectedShapeIndex);
        
        //If central position and all axes are good, it is valid
        if (valid)
        {
            mShapes[mSelectedShapeIndex]->mAlpha = 1.0f;
            mSelectedShapeIndex = -1;
        }
        else
            cout << "INVALID POSITION" << endl;
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT) && mSelectedShapeIndex != -1)
    {
        glm::vec3 translate = glm::normalize(mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f));
        mShapes[mSelectedShapeIndex]->mTranslate += SHAPE_MOVEMENT_SPEED * translate;
        mShapes[mSelectedShapeIndex]->mCenter += SHAPE_MOVEMENT_SPEED * translate;
        mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
        mShapes[mSelectedShapeIndex]->mBox.Set();

        if (!GetIsValidObjectPosition(mSelectedShapeIndex))
        {
            mShapes[mSelectedShapeIndex]->mValidPos = false;
        }
        else
        {
            mShapes[mSelectedShapeIndex]->mValidPos = true;
        }

        if (mDebug)
        {
            std::cout << "Max X: " << mShapes[mSelectedShapeIndex]->mBox.mMax.x << " Min X: " << mShapes[mSelectedShapeIndex]->mBox.mMin.x << endl;
            std::cout << "Max Z: " << mShapes[mSelectedShapeIndex]->mBox.mMax.z << " Min Z: " << mShapes[mSelectedShapeIndex]->mBox.mMin.z << endl;
        }
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT) && mSelectedShapeIndex != -1)
    {
        glm::vec3 translate = glm::normalize(mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f));
        mShapes[mSelectedShapeIndex]->mTranslate += -SHAPE_MOVEMENT_SPEED * translate;
        mShapes[mSelectedShapeIndex]->mCenter += -SHAPE_MOVEMENT_SPEED * translate;
        mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
        mShapes[mSelectedShapeIndex]->mBox.Set();

        if (!GetIsValidObjectPosition(mSelectedShapeIndex))
        {
            mShapes[mSelectedShapeIndex]->mValidPos = false;
        }
        else
        {
            mShapes[mSelectedShapeIndex]->mValidPos = true;
        }

        if (mDebug)
        {
            std::cout << "Max X: " << mShapes[mSelectedShapeIndex]->mBox.mMax.x << " Min X: " << mShapes[mSelectedShapeIndex]->mBox.mMin.x << endl;
            std::cout << "Max Z: " << mShapes[mSelectedShapeIndex]->mBox.mMax.z << " Min Z: " << mShapes[mSelectedShapeIndex]->mBox.mMin.z << endl;
        }
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT) && mSelectedShapeIndex != -1)
    {
        if (mode == GLFW_MOD_SHIFT)
        {
            mShapes[mSelectedShapeIndex]->mRotate += -ROTATION_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
            mShapes[mSelectedShapeIndex]->mBox.mRotate = mShapes[mSelectedShapeIndex]->mRotate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }
        else
        {
            glm::vec3 translate = glm::normalize(mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += -SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mCenter += -SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }

        if (!GetIsValidObjectPosition(mSelectedShapeIndex))
        {
            mShapes[mSelectedShapeIndex]->mValidPos = false;
        }
        else
        {
            mShapes[mSelectedShapeIndex]->mValidPos = true;
        }

        if (mDebug)
        {
            std::cout << "Max X: " << mShapes[mSelectedShapeIndex]->mBox.mMax.x << " Min X: " << mShapes[mSelectedShapeIndex]->mBox.mMin.x << endl;
            std::cout << "Max Z: " << mShapes[mSelectedShapeIndex]->mBox.mMax.z << " Min Z: " << mShapes[mSelectedShapeIndex]->mBox.mMin.z << endl;
            std::cout << "Rotate Y: " << mShapes[mSelectedShapeIndex]->mRotate.y << " Box Rotate Y: " << mShapes[mSelectedShapeIndex]->mBox.mRotate.y << endl;
        }
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT) && mSelectedShapeIndex != -1)
    {
        if (mode == GLFW_MOD_SHIFT)
        {
            mShapes[mSelectedShapeIndex]->mRotate += ROTATION_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
            mShapes[mSelectedShapeIndex]->mBox.mRotate = mShapes[mSelectedShapeIndex]->mRotate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }
        else
        {
            glm::vec3 translate = glm::normalize(mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mCenter += SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }

        if (!GetIsValidObjectPosition(mSelectedShapeIndex))
        {
            mShapes[mSelectedShapeIndex]->mValidPos = false;
        }
        else
        {
            mShapes[mSelectedShapeIndex]->mValidPos = true;
        }

        if (mDebug)
        {
            std::cout << "Max X: " << mShapes[mSelectedShapeIndex]->mBox.mMax.x << " Min X: " << mShapes[mSelectedShapeIndex]->mBox.mMin.x << endl;
            std::cout << "Max Z: " << mShapes[mSelectedShapeIndex]->mBox.mMax.z << " Min Z: " << mShapes[mSelectedShapeIndex]->mBox.mMin.z << endl;
        }
    }
}

void OpenGLWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // SELECT object
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mSelectedShapeIndex == -1)
    {
        float t = 0.0f;
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        glm::mat4 projection_matrix = glm::perspective(45.0f, GetAspectRatio(), 0.01f, 1000.0f);
        glm::mat4 view_matrix = mCamera->GetViewMatrix();
        glm::vec3 camera_position = mCamera->GetPosition();

        glm::vec4 device_ray = glm::vec4(((2.0f * x / mWidth) - 1.0f), (1.0f - (2.0f * y / mHeight)), -1.0f, 1.0f);
        glm::vec4 eye_ray = glm::inverse(projection_matrix) * device_ray;
        eye_ray = glm::vec4(eye_ray.x, eye_ray.y, -1.0f, 0.0f);
        glm::vec3 world_ray = glm::normalize(glm::vec3(glm::inverse(view_matrix) * eye_ray));

        for (unsigned int i = 0; i < mShapes.size(); i++)
        {
            float temp = mShapes[i]->IsSelected(world_ray, camera_position);

            if (mSelectedShapeIndex < 0 && temp >= 0)
            {
                mSelectedShapeIndex = i;
                t = temp;
            }
            else if (temp >= 0 && temp < t)
            {
                mSelectedShapeIndex = i;
                t = temp;
            }
        }

        if (mSelectedShapeIndex != -1)
        {
            mShapes[mSelectedShapeIndex]->mAlpha = 0.5f;
        }
    }

    mOrient = button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS;
    if (mOrient)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        mCamera->SetMousePosition(glm::vec2(x, y));
    }
}

void OpenGLWindow::JoystickCallback(GLFWwindow* window)
{
    int count;
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
    if ((axes[4] < 0.0f - BIAS) || (axes[4] > 0.0f + BIAS) || (axes[3] < 0.0f - BIAS) || (axes[3] > 0.0f + BIAS))
    {
        mCamera->JoystickSetLookAt(glm::vec2(axes[4], axes[3]));
    }
    if (axes[0] < 0.0f - BIAS)
    {
        glm::vec3 desiredPos = mCamera->GetPosition() - CAMERA_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));
    }
    else if (axes[0] > 0.0f + BIAS)
    {
        glm::vec3 desiredPos = mCamera->GetPosition() + CAMERA_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));
    }
    if (axes[1] < 0.0f - BIAS)
    {
        glm::vec3 desiredPos = mCamera->GetPosition() + CAMERA_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));
    }
    else if (axes[1] > 0.0f + BIAS)
    {
        glm::vec3 desiredPos = mCamera->GetPosition() - CAMERA_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f);
        bool valid = false;
        mCamera->SetPosition(GetNoCollisionPosition(mCamera->GetPosition(), desiredPos, valid));
    }
    if (mSelectedShapeIndex != -1)
    {
        if (buttons[10] == GLFW_PRESS)
        {
            glm::vec3 translate = glm::normalize(mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mCenter += SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }
        if (buttons[11] == GLFW_PRESS)
        {
            glm::vec3 translate = glm::normalize(mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mCenter += SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }
        if (buttons[12] == GLFW_PRESS)
        {
            glm::vec3 translate = glm::normalize(mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += -SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mCenter += -SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }
        if (buttons[13] == GLFW_PRESS)
        {
            glm::vec3 translate = glm::normalize(mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += -SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mCenter += -SHAPE_MOVEMENT_SPEED * translate;
            mShapes[mSelectedShapeIndex]->mBox.mTranslate = mShapes[mSelectedShapeIndex]->mTranslate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }

        if (buttons[4] == GLFW_PRESS)
        {
            mShapes[mSelectedShapeIndex]->mRotate += ROTATION_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
            mShapes[mSelectedShapeIndex]->mBox.mRotate = mShapes[mSelectedShapeIndex]->mRotate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }

        if (buttons[5] == GLFW_PRESS)
        {
            mShapes[mSelectedShapeIndex]->mRotate += -ROTATION_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
            mShapes[mSelectedShapeIndex]->mBox.mRotate = mShapes[mSelectedShapeIndex]->mRotate;
            mShapes[mSelectedShapeIndex]->mBox.Set();
        }
        if (!GetIsValidObjectPosition(mSelectedShapeIndex))
        {
            mShapes[mSelectedShapeIndex]->mValidPos = false;
        }
        else
        {
            mShapes[mSelectedShapeIndex]->mValidPos = true;
        }
        if (mDebug)
        {
            std::cout << "Max X: " << mShapes[mSelectedShapeIndex]->mBox.mMax.x << " Min X: " << mShapes[mSelectedShapeIndex]->mBox.mMin.x << endl;
            std::cout << "Max Z: " << mShapes[mSelectedShapeIndex]->mBox.mMax.z << " Min Z: " << mShapes[mSelectedShapeIndex]->mBox.mMin.z << endl;
        }
    }

    if (buttons[0] == GLFW_PRESS && mPrevButtonAState != buttons[0])
    {
        cout << mCamera->GetPosition().x << ", " << mCamera->GetPosition().z << endl;
        if (mSelectedShapeIndex != -1)
        {
            bool valid = GetIsValidObjectPosition(mSelectedShapeIndex);
            //If central position and all axes are good, it is valid
            if (valid)
            {
                mShapes[mSelectedShapeIndex]->mAlpha = 1.0f;
                mSelectedShapeIndex = -1;
            }
            else
            {
                cout << "INVALID POSITION" << endl;
            }
        }
        else
        {
            float t = 0.0f;
            float x = mWidth / 2.0f;
            float y = mHeight / 2.0f;

            glm::mat4 projection_matrix = GetProjectionMatrix();
            glm::mat4 view_matrix = mCamera->GetViewMatrix();
            glm::vec3 camera_position = mCamera->GetPosition();

            glm::vec4 device_ray = glm::vec4(((2.0f * x / mWidth) - 1.0f), (1.0f - (2.0f * y / mHeight)), -1.0f, 1.0f);
            glm::vec4 eye_ray = glm::inverse(projection_matrix) * device_ray;
            eye_ray = glm::vec4(eye_ray.x, eye_ray.y, -1.0f, 0.0f);
            glm::vec3 world_ray = glm::normalize(glm::vec3(glm::inverse(view_matrix) * eye_ray));

            for (unsigned int i = 0; i < mShapes.size(); i++)
            {
                float temp = mShapes[i]->IsSelected(world_ray, camera_position);

                if (mSelectedShapeIndex < 0 && temp >= 0)
                {
                    mSelectedShapeIndex = i;
                    t = temp;
                }
                else if (temp >= 0 && temp < t)
                {
                    mSelectedShapeIndex = i;
                    t = temp;
                }
            }

            if (mSelectedShapeIndex != -1)
            {
                mShapes[mSelectedShapeIndex]->mAlpha = 0.5f;
            }
        }
    }
    if (buttons[3] == GLFW_PRESS && mPrevButtonYState != buttons[3])
    {
        if (buttons[8] == GLFW_PRESS)
        {
            mToggleLight = !mToggleLight;
            if (mToggleLight)
                mLightSwitch = glm::vec3(1.0f);
            else
                mLightSwitch = glm::vec3(0.0f);
        }
        else
        {
            int room = GetCurrentRoom(mCamera->GetPosition());
            if (room <= 3)
            {
                if (mLightSwitch[room - 1])
                    mLightSwitch[room - 1] = 0.0f;
                else
                    mLightSwitch[room - 1] = 1.0f;
            }
        }
    }

    if (buttons[6] == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (buttons[7] == GLFW_PRESS && mPrevStartButtonState != buttons[7])
        mMenuToggle = !mMenuToggle;

    mPrevStartButtonState = buttons[7];
    mPrevButtonYState = buttons[3];
    mPrevButtonAState = buttons[0];
}
