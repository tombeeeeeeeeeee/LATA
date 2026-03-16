#pragma once
#include "Scene.h"

#include "Lights.h"

class EmptyScene : public Scene
{
private:

public:
	




	EmptyScene();
	void Start() override;
	void Update(float delta) override;
	void BeforeDraw(float delta) override;
	void AfterDraw(float delta) override;
	//	void OnMouseDown() override;
	void GUI() override;
	//	void OnWindowResize() override;
	//
	~EmptyScene() override;

};

