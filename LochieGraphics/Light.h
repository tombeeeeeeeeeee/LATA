#pragma once

#include "Shader.h"

#include "Maths.h"

struct Light
{
    glm::vec3 colour;

    float shadowNearPlane = 1.f;
    float shadowFarPlane = 100.0f;
    // pow(2, 10) is 1024
    unsigned int shadowTexWidth = (unsigned int)pow(2, 12);
    unsigned int shadowTexHeight = (unsigned int)pow(2, 12);

    unsigned int shadowFramebuffer = 0;

    Light(glm::vec3 _colour);
    virtual void ApplyToShader(Shader* shader) = 0;
    virtual glm::vec3 getPos() const = 0;
    virtual glm::mat4 getShadowProjection() const = 0;
    virtual glm::mat4 getShadowView() const = 0;
    virtual glm::mat4 getShadowViewProjection() const;

    virtual void GUI();
};

struct DirectionalLight : public Light {
    glm::vec3 direction;

    float fakePosDistance = 10.f;
    float projectionWidth = 15.0f;
    
    DirectionalLight(glm::vec3 _colour, glm::vec3 _direction);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;

    glm::vec3 getPos() const override;
    glm::mat4 getShadowProjection() const override;
    glm::mat4 getShadowView() const override;
};

struct PointLight : public Light {
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    float range = 0;
    // TODO: The index of the light shouldn't be stored here
    int index;
    PointLight(glm::vec3 _colour, glm::vec3 _position, float _constant, float _linear, float _quadratic, int _index);
    PointLight(glm::vec3 _colour, glm::vec3 _position, float range, int _index);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;

    float projectionFov = 150.f;
    glm::vec3 getPos() const override;
    glm::mat4 getShadowProjection() const override;
    glm::mat4 getShadowView() const override;
    void SetRange(float range);
    void SetRange(float linear, float quadratic);
};

struct Spotlight : public PointLight {
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    Spotlight(glm::vec3 _colour, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff);
    Spotlight(glm::vec3 _colour, glm::vec3 _position, float _range, glm::vec3 _direction, float _cutOff, float _outerCutOff);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;

    //glm::mat4 getShadowProjection() const override;
    //glm::mat4 getShadowView() const override;
    //glm::mat4 getShadowViewProjection() const override;
};
