#pragma once
#include "Animator.h"

class BlendedAnimator : public Animator
{
    Animation* otherCurrentAnimation = nullptr;
    unsigned long long otherCurrentAnimationGUID = 0;
    float currentTimeOther = 0.f;
public:

    // TODO: implement in all the types of animators
    bool loopCurrent = true;
    bool loopOtherCurrent = true;

    float lerpAmount = 0.f;
    BlendedAnimator() = default;
    BlendedAnimator(Animation* one, Animation* two);

    void PlayOtherAnimation(Animation* animation);
    void SwitchToAnimation(Animation* animation);

    const Animation* getOtherAnimation() const;

    float getOtherTime() const;

    void UpdateAnimation(float delta) override;
    void CalculateBoneTransform(const ModelHierarchyInfo* node, const glm::mat4& parentTransform) override;

    Animator::Type getType() const override;

    BlendedAnimator(toml::table table);
    toml::table Serialise(unsigned long long GUID) const;

    void GUI() override;

    void BaseGUI() override;
};

