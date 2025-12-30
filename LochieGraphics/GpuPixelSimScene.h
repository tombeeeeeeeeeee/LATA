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

	
	int width = 100;
	int height = 100;

	// this will be the ssbo that holds all the pixel data
	unsigned int ssbo = -1;

	Texture* texture = nullptr;
	FrameBuffer* frameBuffer = nullptr;

	ComputeShader* temp1 = nullptr;

	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;

	Mesh quad;

public:
	//GpuPixelSimScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	//~GpuPixelSimScene() override;

};

