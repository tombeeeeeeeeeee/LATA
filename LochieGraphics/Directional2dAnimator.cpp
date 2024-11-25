#include "Directional2dAnimator.h"

#include "Animation.h"
#include "ModelHierarchyInfo.h"
#include "BoneInfo.h"
#include "Model.h"
#include "ResourceManager.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

Directional2dAnimator::Directional2dAnimator() : Animator()
{
}

Directional2dAnimator::Directional2dAnimator(Animation* left, Animation* right, Animation* up, Animation* down) :
	Animator(left),
	rightAnimation(right),
	upAnimation(up),
	downAnimation(down)
{
}

Directional2dAnimator::Directional2dAnimator(toml::table table) : Animator(table)
{
	rightAnimationGUID = Serialisation::LoadAsUnsignedLongLong(table["rightAnimationGUID"]);
	upAnimationGUID = Serialisation::LoadAsUnsignedLongLong(table["upAnimationGUID"]);
	downAnimationGUID = Serialisation::LoadAsUnsignedLongLong(table["downAnimationGUID"]);

	rightAnimation = ResourceManager::GetAnimation(rightAnimationGUID);
	upAnimation = ResourceManager::GetAnimation(upAnimationGUID);
	downAnimation = ResourceManager::GetAnimation(downAnimationGUID);
}

void Directional2dAnimator::UpdateAnimation(float delta)
{
	if (!currentAnimation || !rightAnimation || !upAnimation || !downAnimation) {
		return;
	}
	currentTime += currentAnimation->getTicksPerSecond() * delta;
	currentTime = fmodf(currentTime, currentAnimation->getDuration());
	currentTimeRight += rightAnimation->getTicksPerSecond() * delta;
	currentTimeRight = fmodf(currentTimeRight, rightAnimation->getDuration());
	currentTimeUp += upAnimation->getTicksPerSecond() * delta;
	currentTimeUp = fmodf(currentTimeUp, upAnimation->getDuration());
	currentTimeDown += downAnimation->getTicksPerSecond() * delta;
	currentTimeDown = fmodf(currentTimeDown, downAnimation->getDuration());

	CalculateBoneTransform(currentAnimation->getRootNode(), glm::mat4(1.0f));
}

void Directional2dAnimator::CalculateBoneTransform(const ModelHierarchyInfo* node, const glm::mat4& parentTransform)
{
	// TODO: All bone matrices get updated even if they don't need to
	std::string nodeName = node->name;
	glm::mat4 globalTransformation;
	Bone* bone1 = currentAnimation->FindBone(nodeName);
	Bone* bone2 = rightAnimation->FindBone(nodeName);
	Bone* bone3 = upAnimation->FindBone(nodeName);
	Bone* bone4 = downAnimation->FindBone(nodeName);

	// These single checks are prob not worth even doing as it should always be the one with all
	if (bone1 && !bone2 && !bone3 && !bone4) {
		bone1->Update(currentTime);
		globalTransformation = parentTransform * bone1->getLocalTransform();
	}
	else if (!bone1 && bone2 && !bone3 && !bone4) {
		bone2->Update(currentTime);
		globalTransformation = parentTransform * bone2->getLocalTransform();
	}
	else if (!bone1 && !bone2 && bone3 && !bone4) {
		bone3->Update(currentTime);
		globalTransformation = parentTransform * bone3->getLocalTransform();
	}
	else if (!bone1 && !bone2 && !bone3 && bone4) {
		bone4->Update(currentTime);
		globalTransformation = parentTransform * bone4->getLocalTransform();
	}
	else if (bone1 && bone2 && bone3 && bone4) {
		bone1->Update(currentTime);
		bone2->Update(currentTimeRight);
		bone3->Update(currentTimeUp);
		bone4->Update(currentTimeDown);

		const glm::mat4& bone1Transform = bone1->getLocalTransform();
		const glm::mat4& bone2Transform = bone2->getLocalTransform();
		const glm::mat4& bone3Transform = bone3->getLocalTransform();
		const glm::mat4& bone4Transform = bone4->getLocalTransform();

		glm::vec3 pos1 = bone1Transform[3];
		glm::vec3 pos2 = bone2Transform[3];
		glm::vec3 pos3 = bone3Transform[3];
		glm::vec3 pos4 = bone4Transform[3];

		glm::quat quat1 = glm::quat_cast(bone1Transform);
		glm::quat quat2 = glm::quat_cast(bone2Transform);
		glm::quat quat3 = glm::quat_cast(bone3Transform);
		glm::quat quat4 = glm::quat_cast(bone4Transform);


		float biasValue = (fabsf(downUpLerpAmount - 0.5f) - fabsf(leftRightLerpAmount - 0.5f)) + 0.5f;

		glm::vec3 leftRightPos = Utilities::Lerp(pos1, pos2, leftRightLerpAmount);
		glm::vec3 downUpPos = Utilities::Lerp(pos4, pos3, downUpLerpAmount);
		glm::vec3 lerpedPos = Utilities::Lerp(leftRightPos, downUpPos, biasValue);

		glm::quat leftRightQuat = glm::slerp(quat1, quat2, leftRightLerpAmount);
		glm::quat downUpQuat = glm::slerp(quat4, quat3, downUpLerpAmount);
		glm::quat slerpedQuat = glm::slerp(leftRightQuat, downUpQuat, biasValue);

		// TODO: Scale

		glm::mat4 m = glm::mat4(1.0f);
		m = glm::translate(m, lerpedPos);
		m = m * glm::mat4_cast(slerpedQuat);

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

void Directional2dAnimator::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Directional 2D Animator##" + tag).c_str())) {
		ImGui::Indent();
		BaseGUI();
		ImGui::Unindent();
	}
}

toml::table Directional2dAnimator::Serialise(unsigned long long GUID) const
{
	toml::table table = Animator::Serialise(GUID);

	table.emplace("rightAnimationGUID", Serialisation::SaveAsUnsignedLongLong(rightAnimationGUID));
	table.emplace("upAnimationGUID", Serialisation::SaveAsUnsignedLongLong(upAnimationGUID));
	table.emplace("downAnimationGUID", Serialisation::SaveAsUnsignedLongLong(downAnimationGUID));

	return table;
}

Animator::Type Directional2dAnimator::getType() const
{
	return Animator::Type::directional2dAnimator;
}

void Directional2dAnimator::BaseGUI()
{
	std::string tag = Utilities::PointerToString(this);
	Animator::BaseGUI();

	if (ResourceManager::AnimationSelector(("Right Animation##" + tag).c_str(), &rightAnimation)) {
		if (rightAnimation) {
			rightAnimationGUID = rightAnimation->GUID;
		}
		else {
			rightAnimationGUID = 0;
		}
	}
	if (rightAnimation) {
		rightAnimation->GUI();
	}
	if (ResourceManager::AnimationSelector(("Up Animation##" + tag).c_str(), &upAnimation)) {
		if (upAnimation) {
			upAnimationGUID = upAnimation->GUID;
		}
		else {
			upAnimationGUID = 0;
		}
	}
	if (upAnimation) {
		upAnimation->GUI();
	}
	if (ResourceManager::AnimationSelector(("Down Animation##" + tag).c_str(), &downAnimation)) {
		if (downAnimation) {
			downAnimationGUID = downAnimation->GUID;
		}
		else {
			downAnimationGUID = 0;
		}
	}
	if (downAnimation) {
		downAnimation->GUI();
	}


	ImGui::SliderFloat(("downUpLerpAmount##" + tag).c_str(), &downUpLerpAmount, 0.0f, 1.0f);
	ImGui::SliderFloat(("leftRightLerpAmount##" + tag).c_str(), &leftRightLerpAmount, 0.0f, 1.0f);


	ImGui::DragFloat(("currentTimeRight##" + tag).c_str(), &currentTimeRight);
	ImGui::DragFloat(("currentTimeUp##" + tag).c_str(), &currentTimeUp);
	ImGui::DragFloat(("currentTimeDown##" + tag).c_str(), &currentTimeDown);

}
