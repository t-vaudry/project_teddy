#include "stdafx.h"
#include "Camera.h"

#include "glm.hpp"
#include "gtx/transform.hpp"
#include "gtc/type_ptr.hpp"

#define ZOOM 0.5f
#define ROTATE 1.0f // degree

Camera::Camera()
    : mPosition(0.0f, 0.0f, 25.0f)
    , mDirection(0.0f, 0.0f, -1.0f)
    , mUp(0.0f, 1.0f, 0.0f)
{
    mRight = glm::cross(mDirection, mUp);
}

void Camera::Pan(const glm::vec2& newMousePosition)
{
    glm::vec2 delta = newMousePosition - mMousePosition;

    if (glm::length(delta) > 50.0f)
    {
        mMousePosition = newMousePosition;
        return;
    }

    mPosition += -0.1f * delta.x * mRight;

    mMousePosition = newMousePosition;
}

void Camera::Tilt(const glm::vec2& newMousePosition)
{
    glm::vec2 delta = newMousePosition - mMousePosition;

    if (glm::length(delta) > 50.0f)
    {
        mMousePosition = newMousePosition;
        return;
    }

    glm::mat3 rotation = glm::mat3(glm::rotate(-0.5f * glm::radians(delta.y), mRight));

    mPosition = rotation * mPosition;
    mDirection = glm::normalize(rotation * mDirection);
    mUp = rotation * mUp;
    mRight = glm::cross(mDirection, mUp);

    mMousePosition = newMousePosition;
}

void Camera::Zoom(const glm::vec2& newMousePosition)
{
    float delta = newMousePosition.y - mMousePosition.y;
    if (delta > 50.0f || delta < -50.0f)
    {
        mMousePosition = newMousePosition;
        return;
    }

    if (delta > 0)
        mPosition -= ZOOM * mDirection;
    else
        mPosition += ZOOM * mDirection;

    mMousePosition = newMousePosition;
}

void Camera::Rotate(ROTATION_AXIS axis, float sign)
{
    glm::vec3 normal;
    switch (axis)
    {
    case X_AXIS:
        normal = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
    case Y_AXIS:
        normal = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case Z_AXIS:
        normal = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    }

    glm::mat3 rotation = glm::mat3(glm::rotate(glm::radians(sign * ROTATE), normal));
    mPosition = rotation * mPosition;
    mDirection = glm::normalize(rotation * mDirection);
    mUp = rotation * mUp;
    mRight = glm::cross(mDirection, mUp);
}

void Camera::MoveCameraBy(const glm::vec3& movementAmount)
{
    mPosition += movementAmount;
}

void Camera::SetLookAt(const glm::vec2& newMousePosition)
{
    float deltaX = 2 * newMousePosition.x / 800.0f - 1; //TODO: Global var
    float deltaY = -(2 * newMousePosition.y / 800.0f - 1);

    //TODO: Will setting fixed position in z work? Seems so
    mDirection = glm::normalize(glm::vec3(deltaX, deltaY, -1.0f));

    mMousePosition = newMousePosition;
}

void Camera::Reset()
{
    mPosition = glm::vec3(25.0f, 10.0f, 25.0f);
    mDirection = glm::vec3(-1.0f, -0.5f, -1.0f);
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mRight = glm::cross(mDirection, mUp);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mDirection, mUp);
}
