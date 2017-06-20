#pragma once
#include <iostream>
#include <glm.hpp>

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

public:
    Camera();

    void Move(DIRECTION direction);
    void JoystickSetLookAt(const glm::vec2& delta);

    inline void SetPosition(glm::vec3 pos) { mPosition = pos; }
    inline glm::vec3 GetPosition() { return mPosition; }

    inline glm::vec3 GetDirection() { return mDirection; }
    inline glm::vec3 GetRight() { return mRight; }

    glm::mat4 GetViewMatrix() const;
};

