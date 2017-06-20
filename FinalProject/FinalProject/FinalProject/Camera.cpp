#include <stdafx.h>
#include <Camera.h>

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>

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

void Camera::Move(DIRECTION direction)
{
    // Move position in direction specified
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

void Camera::JoystickSetLookAt(const glm::vec2& delta)
{
    // Determine rotation based on delta from joystick
    glm::mat4 rotation = glm::rotate(-delta.x * JOYSTICK_ROTATE, mUp) * glm::rotate(-delta.y * JOYSTICK_ROTATE, mRight);

    glm::vec3 tempDirection = glm::normalize(glm::mat3(rotation) * mDirection);

    // Avoid front and back flip of camera at limits close to mUp
    if ((acos(glm::dot(tempDirection, mUp)) >= glm::radians(10.0f)) && (acos(glm::dot(tempDirection, -mUp)) >= glm::radians(10.0f)))
        mDirection = tempDirection;

    mRight = glm::normalize(glm::cross(mDirection, mUp));
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mDirection, mUp);
}
