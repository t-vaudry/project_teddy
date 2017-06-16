#include "stdafx.h"
#include "Camera.h"

#include "glm.hpp"
#include "gtx/transform.hpp"
#include "gtc/type_ptr.hpp"

#define ZOOM 0.5f
#define ROTATE 0.001f
#define JOYSTICK_ROTATE 0.1f
#define MOVEMENT_SPEED 0.1f

Camera::Camera()
    : mPosition(28.0f, -0.25f, 28.0f)
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

void Camera::Move(DIRECTION direction)
{
    switch (direction)
    {
    case FORWARD:
        mPosition += MOVEMENT_SPEED * mDirection * glm::vec3(1.0f, 0.0f, 1.0f);
        break;
    case BACKWARD:
        mPosition += -MOVEMENT_SPEED * mDirection * glm::vec3(1.0f, 0.0f, 1.0f);
        break;
    case LEFT:
        mPosition += -MOVEMENT_SPEED * mRight * glm::vec3(1.0f, 0.0f, 1.0f);
        break;
    case RIGHT:
        mPosition += MOVEMENT_SPEED * mRight * glm::vec3(1.0f, 0.0f, 1.0f);
        break;
    };
}

void Camera::SetLookAt(const glm::vec2& newMousePosition)
{
    glm::vec2 delta = newMousePosition - mMousePosition;

    glm::mat4 rotation = glm::rotate(-delta.x * ROTATE, mUp) * glm::rotate(-delta.y * ROTATE, mRight);
    mDirection = glm::normalize(glm::mat3(rotation) * mDirection);

    mRight = glm::normalize(glm::cross(mDirection, mUp));

    mMousePosition = newMousePosition;
}

void Camera::JoystickSetLookAt(const glm::vec2& delta)
{
    glm::mat4 rotation = glm::rotate(-delta.x * JOYSTICK_ROTATE, mUp) * glm::rotate(-delta.y * JOYSTICK_ROTATE, mRight);
    mDirection = glm::normalize(glm::mat3(rotation) * mDirection);

    mRight = glm::normalize(glm::cross(mDirection, mUp));
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
