#pragma once

#include "Shader.h"

#include "Maths.h"

struct Light
{
    // TODO: Could probably just do with a single colour for the light
    glm::vec3 ambient; // TODO: Don't keep ambient per light
    glm::vec3 diffuse;
    glm::vec3 specular;

    float shadowNearPlane = 1.f;
    float shadowFarPlane = 100.0f;
    // pow(2, 10) is 1024
    unsigned int shadowTexWidth = (unsigned int)pow(2, 12);
    unsigned int shadowTexHeight = (unsigned int)pow(2, 12);

    Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
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
    
    DirectionalLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction);
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
    // TODO: The index of the light shouldn't be stored here
    int index;
    PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, int _index);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;

    float projectionFov = 150.f;
    glm::vec3 getPos() const override;
    glm::mat4 getShadowProjection() const override;
    glm::mat4 getShadowView() const override;
};

struct Spotlight : public PointLight {
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    Spotlight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;

    //glm::mat4 getShadowProjection() const override;
    //glm::mat4 getShadowView() const override;
    //glm::mat4 getShadowViewProjection() const override;
};
