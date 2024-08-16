#pragma once
#include "PointLight.h"

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
    toml::table Serialise() const override;

    Type getType() const;
};
