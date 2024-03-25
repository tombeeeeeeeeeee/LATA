#pragma once

class SceneObject;

class Part
{
public:
	SceneObject* sceneObject;

	virtual void GUI(Part* part) {};

	virtual void Update() {};
	virtual void Draw() {};
};