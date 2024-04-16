#include "Animator.h"

Animator::Animator(Animation* animation) :
    currentAnimation(animation),
    currentTime(0.0f)
{
    //TODO: A plain 100 shouldn't be here
    finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++) {
        finalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::UpdateAnimation(float dt)
{
    if (!currentAnimation) {
        return;
    }
    currentTime += currentAnimation->getTicksPerSecond() * dt;
    currentTime = fmod(currentTime, currentAnimation->getDuration());
    CalculateBoneTransform(&currentAnimation->getRootNode(), glm::mat4(1.0f));
}

void Animator::PlayAnimation(Animation* animation)
{
    currentAnimation = animation;
    currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(currentTime);
        nodeTransform = Bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto& boneInfoMap = currentAnimation->getBoneIDMap();
    auto boneInfo = boneInfoMap.find(nodeName);
    if (boneInfo != boneInfoMap.end())
    {
        int index = boneInfo->second.id;
        glm::mat4 offset = boneInfo->second.offset;
        finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->children.size(); i++) {
        CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}

const std::vector<glm::mat4>& Animator::getFinalBoneMatrices()
{
    return finalBoneMatrices;
}
