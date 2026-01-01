#pragma once
#include "Scene.h"

#include "ComputeShader.h"
#include "Mesh.h"

class GpuPixelSimScene : public Scene
{
private:

	struct CellPixel
	{
		int matId;
	};

	bool update = false;
	bool updateOnce = false;
	
	int width = 100;
	int height = 100;

	int placingMatID = 0;

	// this will be the ssbo that holds all the pixel data
	unsigned int ssbo1 = -1;
	unsigned int ssbo2 = -1;

	unsigned int* readSsbo = nullptr;
	unsigned int* writeSsbo = nullptr;

	Texture* texture = nullptr;
	FrameBuffer* frameBuffer = nullptr;

	ComputeShader* updatePixels = nullptr;
	ComputeShader* placeCircle = nullptr;
	ComputeShader* testCompute = nullptr;

	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;

	Mesh quad;

	void LoadShaders();

	void BindCorrectReadWriteSSBOs();
	void SwitchReadWriteSSBOs();

public:
	//GpuPixelSimScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	//~GpuPixelSimScene() override;

};

