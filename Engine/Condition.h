#pragma once

class SceneObject;

class Condition
{
public:
	virtual bool IsTrue(SceneObject* so) const = 0;
};
