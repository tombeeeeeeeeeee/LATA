//#include "Lophics.h"
#include "SceneManager.h"
#include "Scene.h"

#include "TestScene.h"

int main()
{
	TestScene scene;
	SceneManager sceneManager(&scene);
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
