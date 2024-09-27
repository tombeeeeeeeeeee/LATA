#include "BlendedAnimator.h"

#include "Animation.h"
#include "ModelHierarchyInfo.h"

BlendedAnimator::BlendedAnimator(Animation* one, Animation* two) : Animator(one),
    otherCurrentAnimation(two)
{
}

void BlendedAnimator::UpdateAnimation(float delta)
{
    if (!currentAnimation) {
        return;
    }
    currentTime += currentAnimation->getTicksPerSecond() * delta;
    currentTime = fmod(currentTime, currentAnimation->getDuration());
    currentTimeOther += otherCurrentAnimation->getTicksPerSecond() * delta;
    currentTimeOther = fmod(currentTimeOther, otherCurrentAnimation->getDuration());
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

    auto& boneInfoMap = currentAnimation->getBoneIDMap();
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