#pragma once
#include "Animator.h"

class BlendedAnimator : public Animator
{
    Animation* otherCurrentAnimation = nullptr;
    unsigned long long otherCurrentAnimationGUID = 0;
    float currentTimeOther = 0.f;
public:
    float lerpAmount = 0.f;
    BlendedAnimator() = default;
    BlendedAnimator(Animation* one, Animation* two);

    void UpdateAnimation(float delta) override;
    void CalculateBoneTransform(const ModelHierarchyInfo* node, glm::mat4 parentTransform) override;

    Animator::Type getType() const override;

    BlendedAnimator(toml::table table);
    toml::table Serialise(unsigned long long GUID) const;
};

