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
    static int SHADOW_DIMENSIONS;
    float cutOff;
    float outerCutOff;

    bool hadNonStaticsLastFrame = true;
    bool castsShadows = true;

    unsigned int frameBuffer = 0;
    unsigned int depthBuffer = 0;

    Spotlight();
    Spotlight(toml::table table);
    ~Spotlight();

    Spotlight(const Spotlight& other) = delete;
    Spotlight& operator=(const Spotlight& other);

    Spotlight(Spotlight&& other);
    Spotlight& operator=(Spotlight&& other);
    
    void Initialise();

    void GUI() override;

    glm::mat4 getProj();
    glm::mat4 getView(glm::mat4 globalTransform);

    toml::table Serialise(unsigned long long guid)const override;
};

