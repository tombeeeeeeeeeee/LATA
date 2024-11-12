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
private:
    static int SHADOW_DIMENSIONS;
public:
    glm::vec3 direction = { -1.0f,0.0f,0.0f };
    float cutOff;
    float outerCutOff;

    bool castsShadows = true;

    unsigned int frameBuffer = 0;
    unsigned int depthBuffer = 0;

    Spotlight();
    Spotlight(toml::table table);
    ~Spotlight();

    Spotlight(const Spotlight& other) = delete;
    Spotlight& operator=(const Spotlight& other) = delete;

    Spotlight(Spotlight&& other);
    Spotlight& operator=(Spotlight&& other);
    
    void Initialise();

    void GUI() override;

    glm::mat4 getProj();
    glm::mat4 getView(glm::mat4 globalTransform);

    toml::table Serialise(unsigned long long guid)const override;
};

