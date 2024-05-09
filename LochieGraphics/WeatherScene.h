#pragma once

#include "Scene.h"

#include "Mesh.h"


#include "Weather.h"


class WeatherScene : public Scene
{
public:

	WeatherScene() {};
	void Start() override;
	void EarlyUpdate() override {};
	void Update(float delta) override;
	void Draw() override;
	void OnMouseDown() override;
	void GUI() override {};

	void OnWindowResize() override {};

	~WeatherScene() override {};


private:

	Weather weather;


	Shader* shader = nullptr;

	Mesh quad;
	
};

