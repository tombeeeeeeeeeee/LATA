#pragma once
#include "Scene.h"

#include "Mesh.h"
#include "FrameBuffer.h"

class Texture;
class Shader;

class RaytracerThingScene : public Scene
{
private:
	FrameBuffer* frameBuffer;
	Texture* texture;
	Shader* tracerShader;
	Shader* simpleShader;
	Mesh quad;

	bool redraw = false;
	bool redrawKeyDown = false;

	float timer = 0.0f;

	void DoRayTraceSetup();

public:
	//RaytracerThingScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	//~RaytracerThingScene() override;

};

