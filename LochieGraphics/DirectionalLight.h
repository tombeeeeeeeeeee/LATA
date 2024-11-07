#pragma once
#include "Light.h"

class DirectionalLight {
public:
    glm::vec3 direction = {0.0f,0.0f,0.0f};
    glm::vec3 colour = {0.0f,0.0f,0.0f};

    DirectionalLight() {};
    DirectionalLight(glm::vec3 _colour, glm::vec3 _direction);
    void GUI();

    toml::table Serialise() const;
};

