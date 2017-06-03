#include "stdafx.h"
#include "Camera.h"

#include "glm.hpp"
#include "gtx/transform.hpp"
#include "gtc/type_ptr.hpp"

#define ZOOM 0.5f
#define ROTATE 1.0f // degree

Camera::Camera()
    : mPosition(0.0f, 0.0f, 5.0f)
    , mDirection(0.0f, 0.0f, -1.0f)
    , mUp(0.0f, 1.0f, 0.0f)
{
    mRight = glm::normalize(glm::cross(mDirection, mUp));
}

void Camera::Set(glm::vec3 position, glm::vec3 direction)
{
    mPosition = position;
    mDirection = glm::normalize(direction);
    mRight = glm::normalize(glm::cross(mDirection, mUp));
}

glm::vec3 Camera::GetPosition()
{
    return mPosition;
}

glm::vec3 Camera::GetLightPosition()
{
    glm::vec3 delta = glm::normalize(mDirection - mPosition);
    return mPosition + delta + glm::vec3(0.0f, 0.5f, 0.0f);
}

void Camera::SetPosition(const glm::vec3 position)
{
    mPosition = position;
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
    //mUp = rotation * mUp; // TODO: return when needed
    mRight = glm::normalize(glm::cross(mDirection, mUp));

    mMousePosition = newMousePosition;
}

void Camera::Turn(const glm::vec2& newMousePosition)
{
    glm::vec2 delta = newMousePosition - mMousePosition;

    if (glm::length(delta) > 50.0f)
    {
        mMousePosition = newMousePosition;
        return;
    }

    glm::mat3 rotation = glm::mat3(glm::rotate(-0.5f * glm::radians(delta.x), glm::vec3(0.0f, 1.0f, 0.0f)));

    mPosition = rotation * mPosition;
    mDirection = glm::normalize(rotation * mDirection);
    //mUp = rotation * mUp; // TODO: return when needed
    mRight = glm::normalize(glm::cross(mDirection, mUp));

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
    mRight = glm::normalize(glm::cross(mDirection, mUp));
}

void Camera::Move(DIRECTION direction)
{
    switch (direction)
    {
    case FORWARD:
        mPosition += 1.0f * glm::vec3(1.0f, 0.0f, 1.0f) * mDirection;
        break;
    case BACKWARD:
        mPosition += -1.0f * glm::vec3(1.0f, 0.0f, 1.0f) * mDirection;
        break;
    case LEFT:
        mPosition += -1.0f * glm::vec3(1.0f, 0.0f, 1.0f) * mRight;
        break;
    case RIGHT:
        mPosition += 1.0f * glm::vec3(1.0f, 0.0f, 1.0f) * mRight;
        break;
    case UP:
        mPosition += 1.0f * glm::vec3(0.0f, 1.0f, 0.0f) * mUp;
        break;
    case DOWN:
        mPosition += -1.0f * glm::vec3(0.0f, 1.0f, 0.0f) * mUp;
        break;
    };
}

void Camera::Reset()
{
    mPosition = glm::vec3(0.0f, 0.0f, 25.0f);
    mDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mRight = glm::normalize(glm::cross(mDirection, mUp));
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mDirection, mUp);
}
