#pragma once
#include "Maths.h"
#include "PointLight.h"
#include <string>

namespace toml {
    inline namespace v3 {
        class table;
    }
}
class Spotlight : public PointLight {
public:
    glm::vec3 direction = { -1.0f,0.0f,0.0f };
    float cutOff;
    float outerCutOff;

    bool castsShadows = true;

    int frameBuffer = 0;
    int depthbuffer = 0;
    glm::mat4 viewProj = glm::zero<glm::mat4>();

    Spotlight() {};
    Spotlight(toml::table table);
    void GUI() override;

    toml::table Serialise(unsigned long long guid)const override;
};

