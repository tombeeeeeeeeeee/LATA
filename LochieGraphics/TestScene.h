#pragma once
#include "Scene.h"

#include "Mesh.h"

class TestScene : public Scene
{
private:

	
	Shader* overlayShader;
	Texture* testTexture;
	Mesh quad;

public:
	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void GUI() override;
	~TestScene() override;

};

