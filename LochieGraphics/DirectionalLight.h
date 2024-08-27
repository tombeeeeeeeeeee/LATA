#pragma once
#include "Light.h"

class DirectionalLight : public Light {
public:
    glm::vec3 direction;

    float fakePosDistance = 10.f;
    float projectionWidth = 15.0f;

    DirectionalLight(glm::vec3 _colour, glm::vec3 _direction);
    void ApplyToShader(Shader* shader) override;
    void GUI() override;

    glm::vec3 getPos() const override;
    glm::mat4 getShadowProjection() const override;
    glm::mat4 getShadowView() const override;

    toml::table Serialise() const override;

    Type getType() const;
};

