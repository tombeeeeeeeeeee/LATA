//#include "Lophics.h"
#include "SceneManager.h"
#include "Scene.h"

#include "TestScene.h"

int main()
{
	TestScene scene;
	SceneManager sceneManager(&scene);
	sceneManager.Run();
}
