#include "Animator.h"

#include "Animation.h"
#include "ModelHierarchyInfo.h"
#include "BoneInfo.h"
#include "Model.h"

#include "ExtraEditorGUI.h"
#include "Serialisation.h"

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

const std::vector<glm::mat4>& Animator::getFinalBoneMatrices()
{
    return finalBoneMatrices;
}

toml::table Animator::Serialise(unsigned long long GUID) const
{
    // TODO: Save the current animation
    return toml::table{
        { "guid", Serialisation::SaveAsUnsignedLongLong(GUID) }
    };
}

Animator::Animator(toml::table table)
{
    // TODO: Write
}

void Animator::GUI()
{

    std::string tag = Utilities::PointerToString(this);
    if (ImGui::CollapsingHeader(("Animator##" + tag).c_str())) {
        ImGui::Indent();
        if (ImGui::Button(("Set all scales to 1!##" + tag).c_str())) {
            for (auto& i : currentAnimation->bones)
            {
                for (auto& k : i.keys)
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
        currentAnimation->GUI();
        ImGui::DragFloat(("Animation Time##" + tag).c_str(), &currentTime);
        ImGui::Unindent();
    }
}
