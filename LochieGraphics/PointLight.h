#pragma once
#include "Light.h"

class PointLight : public Light {
public:
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

    toml::table Serialise() const override;

    Type getType() const;
};
