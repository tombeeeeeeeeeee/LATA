#pragma once
#include "Animator.h"


class Directional2dAnimator : public Animator
{
    // Animator current animation is the left animation
    Animation* rightAnimation = nullptr;
    Animation* upAnimation = nullptr;
    Animation* downAnimation = nullptr;
    float currentTimeRight = 0.f;
    float currentTimeUp = 0.f;
    float currentTimeDown = 0.f;

    unsigned long long rightAnimationGUID = 0;
    unsigned long long upAnimationGUID = 0;
    unsigned long long downAnimationGUID = 0;
public:
    // 0 is down, up is 1
    float downUpLerpAmount = 0.5f;
    // 0 is left, right is 1
    float leftRightLerpAmount = 0.5f;
    Directional2dAnimator();
    Directional2dAnimator(Animation* left, Animation* right, Animation* up, Animation* down);
    Directional2dAnimator(toml::table table);

    void UpdateAnimation(float delta) override;
    void CalculateBoneTransform(const ModelHierarchyInfo* node, const glm::mat4& parentTransform) override;

    virtual void GUI() override;

    toml::table Serialise(unsigned long long GUID) const override;

    Animator::Type getType() const override;

protected:

    void BaseGUI() override;
};

