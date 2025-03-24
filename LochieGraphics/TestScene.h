#pragma once
#include "Scene.h"

#include "Mesh.h"

#include "PixelStuff.h"





class TestScene : public Scene
{
private:

	PixelStuff pixelStuff;

	bool updateSim = true;

	


	FrameBuffer* frameBuffer;
	Texture* texture;

	unsigned int ssbo;

	Shader* overlayShader;
	Shader* pixelShader;
	Mesh quad;

	// GUI Stuff

	bool colourSelectedPixel = false;

	glm::vec3 pickerColour;
	glm::ivec2 guiCursor = { 2, 2 };

	float selectEditRadius = 1.0f;

	unsigned int selectMat = 1;

public:
	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	~TestScene() override;

};

