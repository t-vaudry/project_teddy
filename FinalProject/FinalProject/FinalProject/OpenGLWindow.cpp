#include "stdafx.h"
#include "OpenGLWindow.h"

// Public static variables
GLRenderMode OpenGLWindow::mRenderMode = GL_TRIANGLES;
int OpenGLWindow::mWidth;
int OpenGLWindow::mHeight;
Camera* OpenGLWindow::mCamera;
vector<Shape*> OpenGLWindow::mShapes;

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

    glm::mat4 model_matrix = glm::mat4(1.0f);

    model_matrix = glm::translate(model_matrix, shape->mTranslate);
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::rotate(model_matrix, glm::radians(shape->mRotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, shape->mScale);

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = glm::perspective(45.0f, GetAspectRatio(), NEAR_PLANE, FAR_PLANE);

    glm::mat4 mvp_matrix;
    mvp_matrix = projection_matrix * view_matrix * model_matrix;

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniform1f(alphaLoc, shape->mAlpha);
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

void OpenGLWindow::RenderInstancedShape(Shape* shape, GLuint program)
{
    glUseProgram(program);
    GLuint vpLoc = glGetUniformLocation(program, "vp_matrix");
    GLuint alphaLoc = glGetUniformLocation(program, "alpha");

    glm::mat4 view_matrix;
    view_matrix = mCamera->GetViewMatrix();

    glm::mat4 projection_matrix;
    projection_matrix = glm::perspective(45.0f, GetAspectRatio(), NEAR_PLANE, FAR_PLANE);

    glm::mat4 vp_matrix;
    vp_matrix = projection_matrix * view_matrix;

    //broadcast the uniform value to the shaders
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp_matrix));
    glUniform1f(alphaLoc, shape->mAlpha);
}

void OpenGLWindow::DrawSkybox(Shape* shape, GLuint* VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
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

glm::mat4 OpenGLWindow::GetProjectionMatrix()
{
    return glm::perspective(45.0f, GetAspectRatio(), NEAR_PLANE, FAR_PLANE);
}

glm::vec3 OpenGLWindow::GetNoCollisionPosition(glm::vec3 startPos, glm::vec3 desiredEndPos)
{
    //Loop through all shapes in world
    //If within BV of shape, return contact point
    //Else, return desiredEndPoint
    glm::vec3 returnPos = desiredEndPos;
    for (unsigned int i = 0; i < mShapes.size(); i++)
    {
        //Get distance from desiredEndPos to center of sphere
        glm::vec3 centreOnPlane = mShapes[i]->mCenter;
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
    {
        if (mRenderMode == GL_TRIANGLES)
            mRenderMode = GL_LINE_STRIP;
        else
            mRenderMode = GL_TRIANGLES;
    }

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

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && mSelectedShapeIndex != -1)
    {
        bool valid = true;

        for (unsigned int i = 0; i < mShapes.size(); i++)
        {
            if (i == mSelectedShapeIndex)
                continue;

            float center_distance = glm::distance(mShapes[i]->mCenter, mShapes[mSelectedShapeIndex]->mCenter);
            float distance = center_distance - mShapes[i]->mRadius - mShapes[mSelectedShapeIndex]->mRadius;

            if (distance < 0)
            {
                valid = false;
                break;
            }
        }

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
        glm::vec3 translate = glm::normalize(SHAPE_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f));
        mShapes[mSelectedShapeIndex]->mTranslate += translate;
        mShapes[mSelectedShapeIndex]->mCenter += translate;
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT) && mSelectedShapeIndex != -1)
    {
        glm::vec3 translate = glm::normalize(-SHAPE_MOVEMENT_SPEED * mCamera->GetDirection() * glm::vec3(1.0f, 0.0f, 1.0f));
        mShapes[mSelectedShapeIndex]->mTranslate += translate;
        mShapes[mSelectedShapeIndex]->mCenter += translate;
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT) && mSelectedShapeIndex != -1)
    {
        if (mode == GLFW_MOD_SHIFT)
        {
            mShapes[mSelectedShapeIndex]->mRotate += -ROTATION_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else
        {
            glm::vec3 translate = glm::normalize(-SHAPE_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += translate;
            mShapes[mSelectedShapeIndex]->mCenter += translate;
        }
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT) && mSelectedShapeIndex != -1)
    {
        if (mode == GLFW_MOD_SHIFT)
        {
            mShapes[mSelectedShapeIndex]->mRotate += ROTATION_SPEED * glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else
        {
            glm::vec3 translate = glm::normalize(SHAPE_MOVEMENT_SPEED * mCamera->GetRight() * glm::vec3(1.0f, 0.0f, 1.0f));
            mShapes[mSelectedShapeIndex]->mTranslate += translate;
            mShapes[mSelectedShapeIndex]->mCenter += translate;
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
}
