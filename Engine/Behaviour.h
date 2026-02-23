#pragma once

class SceneObject;

class Behaviour
{
public:
	virtual void Enter(SceneObject* so) {};
	virtual void Update(SceneObject* so, float delta) = 0;
	virtual void Exit(SceneObject* so) {};
};

