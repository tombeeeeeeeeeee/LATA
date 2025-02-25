//#include "Lophics.h"
#include "SceneManager.h"
#include "Scene.h"

int main()
{
	Scene scene;
	SceneManager sceneManager(&scene);
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
