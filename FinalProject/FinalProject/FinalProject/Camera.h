#pragma once
#include <iostream>
#include <glm.hpp>

typedef enum
{
    X_AXIS,
    Y_AXIS,
    Z_AXIS
} ROTATION_AXIS;

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
    void MoveCameraBy(const glm::vec3& movementAmount);
    void SetLookAt(const glm::vec2& newMousePosition);
    void Reset();

    glm::mat4 GetViewMatrix() const;
};

