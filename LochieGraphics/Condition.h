#pragma once
struct Condition
{
public:
	virtual bool IsTrue(SceneObject* sceneObject) = 0;
};

