#pragma once
#include "Scene.h"

#include "ComputeShader.h"
#include "Mesh.h"

namespace PixelsGPU
{
	class Simulation
	{
	public:
		// this will be the ssbo that holds all the pixel data
		unsigned int ssbo1 = -1;
		unsigned int ssbo2 = -1;
		unsigned int* readSsbo = nullptr;
		unsigned int* writeSsbo = nullptr;
	
		int width = 1000;
		int height = 1000;

		ComputeShader* updatePixels = nullptr;
		ComputeShader* placeCircle = nullptr;
		ComputeShader* testCompute = nullptr;
		void LoadComputeShaders();
		void BindCorrectReadWriteSSBOs();
		void SwitchReadWriteSSBOs();

		void Initialise();
		void Update(float delta);

		void SetCircleToMaterial(glm::ivec2 pos, float radius, int matID);
	};

	struct CellPixel
	{
		int matId;
	};
	
}

class GpuPixelSimScene : public Scene
{
private:

	PixelsGPU::Simulation pixelSim;

	bool update = false;
	bool updateOnce = false;

	int placingMatID = 0;
	float placingRadius = 5;

	Texture* texture = nullptr;
	FrameBuffer* frameBuffer = nullptr;

	Shader* pixelShader = nullptr;
	Shader* simple2dShader = nullptr;

	Mesh quad;

	void LoadShaders();


public:
	//GpuPixelSimScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	//~GpuPixelSimScene() override;

};

