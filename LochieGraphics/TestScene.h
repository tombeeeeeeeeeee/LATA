#pragma once
#include "Scene.h"

#include "Mesh.h"

#include "PixelStuff.h"


class TestScene : public Scene
{
private:
	PixelStuff pixelStuff;

	bool updateSim = true;

	FrameBuffer* frameBuffer = nullptr;
	Texture* texture = nullptr;

	unsigned int ssbo;

	Shader* overlayShader = nullptr;
	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;
	Mesh quad;

	// GUI Stuff

	bool colourSelectedPixel = false;

	glm::vec3 pickerColour = { 1.0f, 1.0f, 1.0f };
	glm::ivec2 guiCursor = { 2, 2 };

	float selectEditRadius = 1.0f;

	unsigned int selectMat = 1;

	glm::vec2 previousCursorPos;

public:
	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	~TestScene() override;

};

