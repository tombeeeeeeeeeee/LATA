#pragma once
#include "Scene.h"

class TestScene : public Scene
{
private:

public:

	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void OnMouseDown() override;
	void GUI() override;
	void OnWindowResize() override;

	~TestScene() override;
};