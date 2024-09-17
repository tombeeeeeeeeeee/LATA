#pragma once
#include <string>
struct Collision;
class SceneObject;

class Exit
{
public:

	void Initialise(SceneObject* so);
	void Update();
	void OnTrigger(Collision collision);

	void GUI();

private:
	bool eccoInExit = false;
	bool syncInExit = false;
	std::string levelToLoad = "";
};

