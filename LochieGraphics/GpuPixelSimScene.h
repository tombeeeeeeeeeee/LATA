#pragma once
#include "Scene.h"

#include "GpuPixels.h"
#include "Mesh.h"

class GpuPixelSimScene : public Scene
{
private:

	PixelsGPU::Simulation pixelSim;

	bool update = true;
	bool updateOnce = false;

	int placingMatID = 0;
	float placingRadius = 5;
	int renderIndex = 0;
	glm::vec2 previousCursorPos;

	Texture* texture = nullptr;
	FrameBuffer* frameBuffer = nullptr;

	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;

	Mesh quad;

	void LoadShaders();

	glm::vec2 ScreenToWorld(glm::vec2 cursor);


public:
	//GpuPixelSimScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	//~GpuPixelSimScene() override;

};

