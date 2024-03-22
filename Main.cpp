//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"

int main()
{
	SceneManager sceneManager(new TestScene());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}

	//lophics.Start();
	//while (!glfwWindowShouldClose(lophics.window)) {
	//	lophics.Update();
	//}
	//lophics.Stop();
}
