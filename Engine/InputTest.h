#pragma once
#include "Scene.h"

#include "Input.h"
#include "Lights.h"

class InputTest : public Scene
{
public:

	Input input;


	InputTest();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	//	void OnMouseDown() override;
	void GUI() override;
	//	void OnWindowResize() override;


};

