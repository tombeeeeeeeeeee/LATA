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

    virtual void GUI();
};

struct DirectionalLight : public Light {
    glm::vec3 direction;
    DirectionalLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;
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
};

struct Spotlight : public PointLight {
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    Spotlight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;
};
