#pragma once
#include "Scene.h"

#include "Mesh.h"

#include "Array2D.h"

class WeatherScene : public Scene
{
public:

	WeatherScene() {};
	void Start() override;
	void EarlyUpdate() override {};
	void Update(float delta) override;
	void Draw() override {};
	void OnMouseDown() override;
	void GUI() override {};

	void OnWindowResize() override {};

	~WeatherScene() override {};


private:
	Array2D<glm::vec2, 20, 20> vectorMap;
	//std::vector<glm::vec2> vectorMap;
	//int collumns = 20;
	//int rows = 20;
	//int getIndex(int row, int col) const;
	//int getRow(int index) const;
	//int getCol(int index) const;
	//void getRowCol(int index, int& row, int& col) const;

	
	Shader* shader;

	Mesh quad;
	
	//std::vector<unsigned char> ConvertToTextureData();

	void Next();

	void SendToShader();
};

