#pragma once
#include "Scene.h"

#include "Mesh.h"

#include "PixelSimulation.h"

#include "GpuPixelSimScene.h"


class TestScene : public Scene
{
private:

	enum class MouseMode : int {
		None,
		Brush,
		Select,
		COUNT
	};

	SceneObject* player;
	glm::vec2 playerVel = { 0.0f, 0.0f };

	MouseMode mouseMode = MouseMode::Brush;

	Pixels::Simulation pixelSim;

	bool updateSim = true;

	bool activePlayer = false;

	FrameBuffer* chunkFrameBuffer = nullptr;
	Texture* chunkTexture = nullptr;

	//FrameBuffer* output = nullptr;
	//Texture* outputTexture = nullptr;

	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;
	Mesh quad;

	// GUI Stuff

	bool colourSelectedPixel = false;

	glm::vec3 pickerColour = { 1.0f, 1.0f, 1.0f };
	glm::ivec2 guiCursor = { 2, 2 };
	glm::ivec2 selectGuiCursor = { 2, 2 };

	float selectEditRadius = 1.0f;

	Pixels::MatID selectMat = 2;

	int lastCount = 0;

	glm::ivec2 previousGuiCursor;

	// TODO: Figure out a spot for this
	std::vector<glm::ivec2> GeneratePathBetween(glm::ivec2 start, glm::ivec2 end);

	glm::vec2 ScreenToWorldPos(const glm::vec2& screenPos);

	void SelectedInfoGUI();
	void StatsGUI();
	void PixelMaterialsGUI();

	void SetupSim();

public:
	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	~TestScene() override;

};

