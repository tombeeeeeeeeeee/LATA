#include "BlendedAnimator.h"

#include "Animation.h"
#include "ModelHierarchyInfo.h"
#include "BoneInfo.h"
#include "Model.h"
#include "ResourceManager.h"

#include "Serialisation.h"

BlendedAnimator::BlendedAnimator(Animation* one, Animation* two) : Animator(one),
    otherCurrentAnimation(two),
    otherCurrentAnimationGUID(two->GUID)
{
}

void BlendedAnimator::PlayOtherAnimation(Animation* animation)
{
    otherCurrentAnimation = animation;
    otherCurrentAnimationGUID = animation->GUID;
    currentTimeOther = 0.0f;
}

void BlendedAnimator::SwitchToAnimation(Animation* animation)
{
    otherCurrentAnimation = currentAnimation;
    otherCurrentAnimationGUID = currentAnimationGUID;
    currentTimeOther = currentTime;
    loopOtherCurrent = loopCurrent;
    PlayAnimation(animation);
}

const Animation* BlendedAnimator::getOtherAnimation() const
{
    return otherCurrentAnimation;
}

float BlendedAnimator::getOtherTime() const
{
    return currentTimeOther;
}

void BlendedAnimator::UpdateAnimation(float delta)
{
    if (!currentAnimation && !otherCurrentAnimation) {
        return;
    }
    // TODO: Don't know this is what we want
    if (currentAnimation && !otherCurrentAnimation) {
        otherCurrentAnimation = currentAnimation;
    }
    if (!currentAnimation && otherCurrentAnimation) {
        currentAnimation = otherCurrentAnimation;
    }
    currentTime += currentAnimation->getTicksPerSecond() * delta;
    currentTimeOther += otherCurrentAnimation->getTicksPerSecond() * delta;
    if (loopCurrent) {
        currentTime = fmodf(currentTime, currentAnimation->getDuration());
    }
    else {
        currentTime = fminf(currentTime, currentAnimation->getDuration());
    }
    if (loopOtherCurrent) {
        currentTimeOther = fmodf(currentTimeOther, otherCurrentAnimation->getDuration());
    }
    else {
        currentTimeOther = fminf(currentTimeOther, currentAnimation->getDuration());
    }
    CalculateBoneTransform(currentAnimation->getRootNode(), glm::mat4(1.0f));
}

void BlendedAnimator::CalculateBoneTransform(const ModelHierarchyInfo* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 globalTransformation;
    Bone* bone1 = currentAnimation->FindBone(nodeName);
    Bone* bone2 = otherCurrentAnimation->FindBone(nodeName);

    if (bone1 && !bone2)
    {
        bone1->Update(currentTime);
        globalTransformation = parentTransform * bone1->getLocalTransform();
    }
    else if (!bone1 && bone2) {
        bone2->Update(currentTimeOther);
        globalTransformation = parentTransform * bone2->getLocalTransform();
    }
    else if (bone1 && bone2){
        bone1->Update(currentTime);
        bone2->Update(currentTimeOther);

        // TODO: Make a 
        // TODO: Make a deconstruct mat4 function (glm's one has too much/not enough)
        const glm::mat4& bone1Transform = bone1->getLocalTransform();
        const glm::mat4& bone2Transform = bone2->getLocalTransform();
        glm::vec3 pos1 = bone1Transform[3];
        glm::vec3 pos2 = bone2Transform[3];

        glm::quat quat1 = glm::quat_cast(bone1Transform);
        glm::quat quat2 = glm::quat_cast(bone2Transform);

        // TODO: there is a lerp function in utils
        //glm::vec3 lerpPos = pos1 + (pos2 - pos1) * lerpAmount;
        glm::vec3 lerpPos = pos1 * (1.f - lerpAmount) + pos2 * lerpAmount;
        glm::quat slerpQuat = glm::slerp(quat1, quat2, lerpAmount);
        // TODO: Scale


        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, lerpPos);
        //m = glm::scale(m, glm::vec3(scale));
        m = m * glm::mat4_cast(slerpQuat);

        globalTransformation = parentTransform * m;
    }
    else {
        globalTransformation = parentTransform * node->transform.getLocalMatrix();
    }

    auto& boneInfoMap = currentAnimation->model->boneInfoMap;
    auto boneInfo = boneInfoMap.find(nodeName);
    if (boneInfo != boneInfoMap.end())
    {
        int index = boneInfo->second.ID;
        glm::mat4 offset = boneInfo->second.offset;
        finalBoneMatrices[index] = globalTransformation * offset;
    }

    // Recursively call on children
    for (int i = 0; i < node->children.size(); i++) {
        CalculateBoneTransform(node->children[i], globalTransformation);
    }
}

Animator::Type BlendedAnimator::getType() const
{
    return Animator::Type::blended;
}

BlendedAnimator::BlendedAnimator(toml::table table) : Animator(table)
{
    otherCurrentAnimationGUID = Serialisation::LoadAsUnsignedLongLong(table["otherCurrentAnimationGUID "]);
    otherCurrentAnimation = ResourceManager::GetAnimation(otherCurrentAnimationGUID);
}

toml::table BlendedAnimator::Serialise(unsigned long long GUID) const
{
    toml::table table = Animator::Serialise(GUID);

    table.emplace("otherCurrentAnimationGUID", Serialisation::SaveAsUnsignedLongLong(otherCurrentAnimationGUID));
    return table;
}
