#pragma once
#include "Scene.h"

#include "GpuPixels.h"
#include "Mesh.h"

class GpuPixelSimScene : public Scene
{
private:

	enum class MouseMode : int {
		None,
		Brush,
		SelectPixel,
		SelectChunk,
		COUNT
	};

	MouseMode mouseMode = MouseMode::Brush;

	PixelsGPU::Simulation pixelSim;

	bool update = true;
	bool updateOnce = false;

	int placingMatID = 0;
	float placingRadius = 5;
	int renderIndex = 0;
	glm::vec2 previousCursorPos;
	glm::ivec2 chunkSelected;

	Texture* texture = nullptr;
	FrameBuffer* frameBuffer = nullptr;

	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;

	Mesh quad;

	void LoadShaders();

	glm::vec2 ScreenToWorld(glm::vec2 cursor);

	void DrawCircle(glm::vec2 worldCurrentCursorPos);


public:
	//GpuPixelSimScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	//~GpuPixelSimScene() override;

};

