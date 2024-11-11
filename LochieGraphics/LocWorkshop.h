#pragma once
#include "Scene.h"

#include "DirectionalLight.h"
#include "Animation.h"
#include "ComputeShader.h"
#include "Mesh.h"
#include "Particle.h"
#include "Directional2dAnimator.h"

class LocWorkshop : public Scene
{
private:


	struct AnimationTestPacket {
		std::string name;
		std::string path;
		Animation animation;
		SceneObject* sceneObject = nullptr;
		Model* model = nullptr;
		AnimationTestPacket(std::string name, std::string path);
	};

	std::vector<AnimationTestPacket> animationTestPackets;
	
	Directional2dAnimator eccoAnimator;
	Animation eccoLeft;
	Animation eccoRight;
	Animation eccoUp;
	Animation eccoDown;
	
	
	Shader* shader;

public:

	LocWorkshop();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	//	void OnMouseDown() override;
	void GUI() override;
	//	void OnWindowResize() override;
	//
	//~LocWorkshop() override;
};

