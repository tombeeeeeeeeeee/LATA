#include "Animator.h"

Animator::Animator(Animation* animation) :
    currentAnimation(animation)
{
    currentTime = 0.0;
    
    finalBoneMatrices.reserve(100);

    for (int i = 0; i < 100; i++) {
        finalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::UpdateAnimation(float dt)
{
    if (currentAnimation)
    {
        currentTime += currentAnimation->getTicksPerSecond() * dt;
        currentTime = fmod(currentTime, currentAnimation->getDuration());
        CalculateBoneTransform(&currentAnimation->getRootNode(), glm::mat4(1.0f));
    }
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

    auto boneInfoMap = currentAnimation->getBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++) {
        CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}

std::vector<glm::mat4> Animator::getFinalBoneMatrices()
{
    return finalBoneMatrices;
}
