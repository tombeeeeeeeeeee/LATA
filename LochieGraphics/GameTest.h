#pragma once
#include "Scene.h"

#include "Input.h"
#include "Lights.h"

#include "Image.h"
#include <array>

class GameTest : public Scene
{
private:

	Input input;

	SceneObject* h;
	SceneObject* r;

	RigidBody* hRb = nullptr;
	RigidBody* rRb = nullptr;

	float hRadius = 0.1f;
	float rRadius = 0.1f;


	float rot = 0.0f;

	Image level;

	float halfGridSpacing = 0.5f;

	bool MapCellIs(unsigned char* cell, unsigned char r, unsigned char g, unsigned char b);

public:


	GameTest();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	//	void OnMouseDown() override;
	void GUI() override;
	//	void OnWindowResize() override;
	//
	~GameTest() override;

private: 

	bool firstFrame = true;

	bool targettingP1 = false;
	bool singlePlayerMode = false;
	float singlePlayerZoom = 0.5f;

	std::array<std::string, 6> irradianceFaces = {
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png",
	"images/otherskybox/nx.png"
	};

};

