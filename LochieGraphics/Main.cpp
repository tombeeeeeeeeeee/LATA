//#include "Lophics.h"
#include "SceneManager.h"
#include "Scene.h"

#include "TestScene.h"
#include "RaytracerThingScene.h"

int main()
{
	RaytracerThingScene scene;
	SceneManager sceneManager(&scene);
	sceneManager.Run();
}
