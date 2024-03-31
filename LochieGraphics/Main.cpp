//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
	SceneManager sceneManager(new TestScene());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
