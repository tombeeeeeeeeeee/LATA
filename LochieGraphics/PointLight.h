#pragma once
#include "Maths.h"
#include <string>

namespace toml {
    inline namespace v3 {
        class table;
    }
}

enum class PointLightEffect
{
    On,
    Off,
    Flickering,
    Explosion,
    SpotLightSpawnRoom,
    OffDelete,
};

class PointLight {
public:
    float linear = 0;
    float quadratic = 0;
    glm::vec3 colour = {1.0f,1.0f,1.0f};
    float intensity = 1.0f;

    bool on = true;
    bool canBeTriggered = false;
    bool castsShadows = true;
    std::string triggerTag = "";

    float range = 1;
    float timeInType = 0.0f;
    PointLightEffect effect = PointLightEffect::On;

    PointLight() { SetRange(100.0f); };
    PointLight(toml::table table);
    PointLight(PointLightEffect _effect) : effect(_effect) {};
    virtual void GUI();

    void SetRange(float range);
    void SetRange(float linear, float quadratic);

    virtual toml::table Serialise(unsigned long long guid)const;

    void TriggerCall(std::string tag, bool toggle);

    static void Attenutation(float range, float& linear, float& quad);
};
