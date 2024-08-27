#include "Animator.h"

#include "ModelHierarchyInfo.h"

Animator::Animator(Animation* animation) :
    currentAnimation(animation)
{
    //TODO: A plain 100 shouldn't be here
    // If there is just a fixed max size than an array might just be better to use
    finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++) {
        finalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::UpdateAnimation(float delta)
{
    if (!currentAnimation) {
        return;
    }
    currentTime += currentAnimation->getTicksPerSecond() * delta;
    currentTime = fmod(currentTime, currentAnimation->getDuration());
    CalculateBoneTransform(currentAnimation->getRootNode(), glm::mat4(1.0f));
}

void Animator::PlayAnimation(Animation* animation)
{
    currentAnimation = animation;
    currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const ModelHierarchyInfo* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 globalTransformation;
    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(currentTime);
        globalTransformation = parentTransform * Bone->getLocalTransform();
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

const std::vector<glm::mat4>& Animator::getFinalBoneMatrices()
{
    return finalBoneMatrices;
}
