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
}
