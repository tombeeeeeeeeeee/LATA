//#include "Lophics.h"
#include "SceneManager.h"
#include "Scene.h"

#include "GpuPixelSimScene.h"

int main()
{
	GpuPixelSimScene scene;
	SceneManager sceneManager(&scene);
	sceneManager.Run();
}
