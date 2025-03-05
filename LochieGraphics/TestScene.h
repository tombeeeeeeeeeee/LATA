#pragma once
#include "Scene.h"

#include "Mesh.h"


enum MaterialFlags {
	neverUpdate = 0, // Prob just doesn't need to exist? (Could exist as a combo of/lack of flags?)
	gravity = 1 << 0
};


struct MaterialInfo {
	std::string name;
	glm::vec4 defaultColour;
	unsigned int flags;
	float density = 0; // Would we want this to be a float or an int?
	MaterialInfo(std::string _name, glm::vec4(_colour), float _density, unsigned int _flags);

	void GUI();
};

struct PixelData {
	glm::vec4 colour = { (float)0xc5 / 0xff, (float)0xde / 0xff, (float)0xe3 / 0xff, 1.0f };
	unsigned int materialID = 0;

	void GUI();
};

// This is intended to always be the same as the version for the shader
struct GpuPixelData {
	glm::vec4 colour;
};


#define PIXELS_W 64
#define PIXELS_H 64

class TestScene : public Scene
{
private:

	bool UpdateSim = true;

	std::array<std::array<PixelData, PIXELS_H>, PIXELS_W> pixels;
	std::array<std::array<GpuPixelData, PIXELS_H>, PIXELS_W> GpuPixels;
	
	std::vector<MaterialInfo> materialInfos;


	FrameBuffer* frameBuffer;
	Texture* texture;

	unsigned int ssbo;

	Shader* overlayShader;
	Shader* pixelShader;
	Mesh quad;

	bool even = true;

	// GUI Stuff

	glm::vec3 pickerColour;
	glm::ivec2 guiCursor = { 2, 2 };

public:
	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	~TestScene() override;

};

