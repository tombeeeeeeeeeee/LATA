//#include "Lophics.h"
#include "SceneManager.h"
#include "Scene.h"

#include "GpuPixelSimScene.h"
#include "TestScene.h"

int main()
{
	GpuPixelSimScene scene;
	SceneManager sceneManager(&scene);
	sceneManager.Run();
}
