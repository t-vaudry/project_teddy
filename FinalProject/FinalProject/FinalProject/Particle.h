#pragma once
#include <glew.h>
#include <glm.hpp>

#define NUM_OF_PARTICLES 1000
#define SPAWN_PARTICLES 100
#define DECAY_FACTOR 0.01f

struct ParticleInfo
{
    ParticleInfo() {}

    glm::vec3 mPosition;
    glm::vec3 mColor;
    glm::vec2 mUV;
    float mAlpha;
};

struct Particle
{
    ParticleInfo mInfo;
    glm::vec3 mVelocity;
    float mLife;

    Particle()
        : mVelocity(glm::vec3(0.0f, -1.0f, 0.0f))
        , mLife(1.0f)
    {
        float r1 = glm::clamp((float) rand() / (float) RAND_MAX, 0.0f, 1.0f);
        float r2 = glm::clamp((float) rand() / (float) RAND_MAX, 0.0f, 1.0f);
        float r3 = glm::clamp((float) rand() / (float) RAND_MAX, 0.0f, 1.0f);
        mInfo.mPosition = glm::vec3(r1, -1.0f, r2);
        mInfo.mColor = glm::vec3(r3);
        mInfo.mUV = glm::vec2(r3);
        mInfo.mAlpha = 1.0f;
    }

    void RespawnParticle()
    {
        float r1 = glm::clamp((float) rand() / (float) RAND_MAX, 0.0f, 1.0f);
        float r2 = glm::clamp((float) rand() / (float) RAND_MAX, 0.0f, 1.0f);
        float r3 = glm::clamp((float) rand() / (float) RAND_MAX, 0.0f, 1.0f);
        mLife = 1.0f;
        mInfo.mPosition = glm::vec3(r1, -1.0f, r2);
        mInfo.mColor = glm::vec3(r3);
        mInfo.mUV = glm::vec2(r3);
        mInfo.mAlpha = 1.0f;
    }
};
