#include "Animator.h"

#include "Animation.h"
#include "ModelHierarchyInfo.h"
#include "BoneInfo.h"
#include "Model.h"
#include "ResourceManager.h"
#include "BlendedAnimator.h"
#include "Directional2dAnimator.h"

#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include <iostream>

Animator::Animator()
{
    // If there is just a fixed max size than an array might just be better to use
    finalBoneMatrices.reserve(MAX_BONES_ON_MODEL);

    for (int i = 0; i < MAX_BONES_ON_MODEL; i++) {
        finalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

Animator::Animator(Animation* animation) : Animator()
{
    currentAnimation = animation;
    if (currentAnimation) {
        currentAnimationGUID = currentAnimation->GUID;
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
    currentAnimationGUID = currentAnimation->GUID;
    currentTime = 0.0f;
}

const Animation* Animator::getAnimation() const
{
    return currentAnimation;
}

float Animator::getTime() const
{
    return currentTime;
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

const std::vector<glm::mat4>& Animator::getFinalBoneMatrices() const
{
    return finalBoneMatrices;
}

std::vector<glm::mat4>& Animator::getNonConstFinalBoneMatrices()
{
    return finalBoneMatrices;
}

toml::table Animator::Serialise(unsigned long long GUID) const
{
    return toml::table{
        { "guid", Serialisation::SaveAsUnsignedLongLong(GUID) },
        { "currentAnimationGUID", Serialisation::SaveAsUnsignedLongLong(currentAnimationGUID) },
        { "type", Serialisation::SaveAsUnsignedInt(getType())},
    };
}

Animator::Animator(toml::table table) : Animator()
{
    currentAnimationGUID = Serialisation::LoadAsUnsignedLongLong(table["currentAnimationGUID"]);
    currentAnimation = ResourceManager::GetAnimation(currentAnimationGUID);
}

Animator::Type Animator::getType() const
{
    return Type::base;
}

void Animator::GUI()
{
    std::string tag = Utilities::PointerToString(this);
    if (ImGui::CollapsingHeader(("Animator##" + tag).c_str())) {
        ImGui::Indent();
        BaseGUI();
        ImGui::Unindent();
    }
}

Animator* Animator::Load(toml::table table)
{
    Animator::Type animatorType = (Animator::Type)Serialisation::LoadAsUnsignedInt(table["type"]);
    switch (animatorType)
    {
    case Animator::Type::base:
        return new Animator(table);
    case Animator::Type::blended:
        return new BlendedAnimator(table);
    case Animator::Type::directional2dAnimator:
        return new Directional2dAnimator(table);
    default:
        std::cout << "Unsupported Collider attempting to load\n";
        return nullptr;
    }
}


void Animator::BaseGUI()
{
    std::string tag = Utilities::PointerToString(this);

    if (ImGui::Button(("Set all scales to 1!##" + tag).c_str())) {
        for (auto& i : currentAnimation->bones)
        {
            for (auto& k : i.scaleKeys)
            {
                k.scale = glm::vec3(1.0f, 1.0f, 1.0f);
            }
        }
    }
    if (ImGui::CollapsingHeader(("Final Bone Matrices##" + tag).c_str())) {
        ImGui::Indent();
        for (size_t i = 0; i < finalBoneMatrices.size(); i++)
        {
            std::string tag = Utilities::PointerToString(&finalBoneMatrices[i]);
            if (ImGui::CollapsingHeader(("Bone ID " + std::to_string(i) + "##" + tag).c_str())) {
                ExtraEditorGUI::Mat4Input(tag, &finalBoneMatrices[i]);
            }
        }
        ImGui::Unindent();
    }
    // TODO: Show the animation
    if (ResourceManager::AnimationSelector(("Animation #1##" + tag).c_str(), &currentAnimation, true)) {
        if (currentAnimation) {
            currentAnimationGUID = currentAnimation->GUID;
        }
        else {
            currentAnimationGUID = 0;
        }
    }
    if (currentAnimation) {
        currentAnimation->GUI();
    }
    ImGui::DragFloat(("Animation Time##" + tag).c_str(), &currentTime);

}
