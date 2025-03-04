#pragma once
#include "Scene.h"

#include "Mesh.h"

// TODO: This is currently required to match the GPU version of the struct, it shouldn't have to be
struct PixelData {
	glm::vec4 colour = { 0.5f, 0.2f, 1.0, 1.0f};
};

#define PIXELS_W 64
#define PIXELS_H 64

class TestScene : public Scene
{
private:

	std::array<std::array<PixelData, PIXELS_H>, PIXELS_W> pixels;

	FrameBuffer* frameBuffer;
	Texture* texture;

	unsigned int ssbo;

	Shader* overlayShader;
	Shader* pixelShader;
	Mesh quad;

public:
	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	~TestScene() override;

};

