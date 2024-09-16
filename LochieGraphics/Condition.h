#pragma once
class SceneObject;

struct Condition
{
public:
	virtual bool IsTrue(SceneObject* sceneObject) = 0;
};

