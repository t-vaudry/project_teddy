#pragma once
#include <iostream>
#include <glm.hpp>

typedef enum
{
    X_AXIS,
    Y_AXIS,
    Z_AXIS
} ROTATION_AXIS;

typedef enum
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
} DIRECTION;

class Camera
{
private:
    glm::vec3 mPosition;
    glm::vec3 mDirection;
    glm::vec3 mUp;
    glm::vec3 mRight;

    glm::vec2 mMousePosition;

public:
    Camera();

    bool mIsTilting = false;
    bool mIsTurning = false;

    void Set(glm::vec3 position, glm::vec3 direction);
    glm::vec3 GetPosition();
    glm::vec3 GetLightPosition();
    void SetPosition(const glm::vec3 position);
    void Pan(const glm::vec2& newMousePosition);
    void Tilt(const glm::vec2& newMousePosition);
    void Turn(const glm::vec2& newMousePosition);
    void Zoom(const glm::vec2& newMousePosition);
    void Rotate(ROTATION_AXIS axis, float sign);
    void Move(DIRECTION direction);
    void Reset();

    glm::mat4 GetViewMatrix() const;
};

