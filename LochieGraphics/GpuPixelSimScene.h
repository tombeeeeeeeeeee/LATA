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
	
		int width = 1600;
		int height = 900;

		float timer = 0.0f;
		int frameCount = 0;

		ComputeShader* updatePixels = nullptr;
		ComputeShader* placeCircle = nullptr;
		ComputeShader* testCompute = nullptr;
		void LoadComputeShaders();
		void BindCorrectReadWriteSSBOs();
		void SwitchReadWriteSSBOs();

		void Initialise();
		void Update(float delta);

		size_t CalculateSsboSize();

		void SetCircleToMaterial(glm::ivec2 pos, float radius, int matID);
		void SetCircleToMaterial(int x, int y, float radius, int matID);
	};

	// Keep this correctly aligned with the gpu version, as thats all this is really for
	// Currently this is only used to be a size representation of the gpu version
	struct alignas(32) CellPixel // Think we would want something other than just alignas
	{
		int matId;
		glm::vec2 vel;
		glm::vec4 colour;
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
	int renderIndex = 0;

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

