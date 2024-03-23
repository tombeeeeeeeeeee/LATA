#pragma once

#include "Shader.h"

#include "Maths.h"

struct Light
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
    virtual void ApplyToShader(Shader* shader) = 0;
};

struct DirectionalLight : public Light {
    glm::vec3 direction;
    DirectionalLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction);
    void ApplyToShader(Shader* shader) override;
};

struct PointLight : public Light {
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic);
    void ApplyToShader(Shader* shader) override;
};

struct SpotLight : public PointLight {
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    SpotLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff);
    void ApplyToShader(Shader* shader) override;
};
