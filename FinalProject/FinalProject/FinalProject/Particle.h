#pragma once
#include <glew.h>
#include <glm.hpp>

#define NUM_OF_PARTICLES 2000
#define SPAWN_PARTICLES 100
#define DECAY_FACTOR 0.1f

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
        float r1 = RandomFloat(0.0f, 0.1f);
        float r2 = RandomFloat(0.0f, 0.1f);
        float r3 = RandomFloat(0.0f, 1.0f);
        mInfo.mPosition = glm::vec3(26.0f + r1, -1.0f, 26.0f + r2);
        mInfo.mColor = glm::vec3(r3);
        mInfo.mUV = glm::vec2(r3);
        mInfo.mAlpha = 1.0f;
    }

    void RespawnParticle()
    {
        float r1 = RandomFloat(0.0f, 0.1f);
        float r2 = RandomFloat(0.0f, 0.1f);
        float r3 = RandomFloat(0.0f, 1.0f);
        mLife = 1.0f;
        mInfo.mPosition = glm::vec3(26.0f + r1, -1.0f, 26.0f + r2);
        mInfo.mColor = glm::vec3(r3);
        mInfo.mUV = glm::vec2(r3);
        mInfo.mAlpha = 1.0f;
    }

    static float RandomFloat(float min, float max) {
        return  (max - min) * ((((float)rand()) / (float)RAND_MAX)) + min;
    }
};
