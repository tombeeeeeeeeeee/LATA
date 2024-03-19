#pragma once

#include "Maths.h"
struct Light
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    Light(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular);
};

struct DirectionalLight : public Light {
    glm::vec3 direction;
    DirectionalLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction);
};

struct PointLight : public Light {
    glm::vec3 position;
    float constant;
    float linear;
    float quadratic;
    PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic);
};

struct SpotLight : public PointLight {
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
    SpotLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _position, float _constant, float _linear, float _quadratic, glm::vec3 _direction, float _cutOff, float _outerCutOff);
};
