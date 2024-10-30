#pragma once
#include "Maths.h"
#include <string>

namespace toml {
    inline namespace v3 {
        class table;
    }
}
class Spotlight{
public:
    float linear = 0;
    float quadratic = 0;
    glm::vec3 colour = { 0.0f,0.0f,0.0f };

    bool on = true;
    bool canBeTriggered = false;
    std::string triggerTag = "";

    float range = 1;

    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    void SetRange(float range);
    void SetRange(float linear, float quadratic);

    Spotlight() {};
    Spotlight(toml::table table);
    void GUI();

    void TriggerCall(std::string tag, bool toggle);

    toml::table Serialise(unsigned long long guid)const;
};
