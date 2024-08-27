#pragma once

#include "Shader.h"

#include "Maths.h"

class FrameBuffer;
class Texture;

struct Light
{
    enum class Type {
        directional,
        spot,
        point
    };


    glm::vec3 colour;

    float shadowNearPlane = 1.f;
    float shadowFarPlane = 100.0f;
    // pow(2, 10) is 1024
    unsigned int shadowTexWidth = (unsigned int)pow(2, 12);
    unsigned int shadowTexHeight = (unsigned int)pow(2, 12);

    // TODO: Ensure these get un loaded
    bool castShadows = false;
    FrameBuffer* shadowFrameBuffer = nullptr;
    Texture* depthMap = nullptr;

    Light(glm::vec3 _colour);
    // For shadow stuff
    void Initialise();
    virtual void ApplyToShader(Shader* shader) = 0;
    virtual glm::vec3 getPos() const = 0;
    virtual glm::mat4 getShadowProjection() const = 0;
    virtual glm::mat4 getShadowView() const = 0;
    virtual glm::mat4 getShadowViewProjection() const;

    virtual void GUI();

    virtual Type getType() const = 0;

    virtual toml::table Serialise() const;
};
