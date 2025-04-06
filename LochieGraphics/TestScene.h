#pragma once
#include "Scene.h"

#include "Mesh.h"

#include "PixelSimulation.h"


class TestScene : public Scene
{
private:
	Pixels::Simulation pixelSim;

	bool updateSim = true;

	FrameBuffer* chunkFrameBuffer = nullptr;
	Texture* chunkTexture = nullptr;

	//FrameBuffer* output = nullptr;
	//Texture* outputTexture = nullptr;

	Shader* overlayShader = nullptr;
	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;
	Mesh quad;

	// GUI Stuff

	bool colourSelectedPixel = false;

	glm::vec3 pickerColour = { 1.0f, 1.0f, 1.0f };
	glm::ivec2 guiCursor = { 2, 2 };

	float selectEditRadius = 1.0f;

	unsigned int selectMat = 2;

	glm::ivec2 previousGuiCursor;

public:
	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	~TestScene() override;

};

