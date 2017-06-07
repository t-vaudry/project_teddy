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
    RIGHT
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

    bool mIsPanning = false;
    bool mIsTilting = false;
    bool mIsZooming = false;

    void Pan(const glm::vec2& newMousePosition);
    void Tilt(const glm::vec2& newMousePosition);
    void Zoom(const glm::vec2& newMousePosition);
    void Rotate(ROTATION_AXIS axis, float sign);
    void Move(DIRECTION direction);
    void SetLookAt(const glm::vec2& newMousePosition);
    void Reset();

    inline void SetPosition(glm::vec3 pos) { mPosition = pos; }
    inline glm::vec3 GetPosition() { return mPosition; }

    inline glm::vec3 GetDirection() { return mDirection; }
    inline glm::vec3 GetRight() { return mRight; }

    glm::mat4 GetViewMatrix() const;
};

