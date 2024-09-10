#pragma once

class SceneObject;
class Shader;

class Part
{
public:
	virtual void GUI() {};

	virtual void Update() {};
	virtual void Draw(Shader* override) {};
};