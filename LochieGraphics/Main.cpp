//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "ArtScene.h"
#include "InputTest.h"


int main()
{
	SceneManager sceneManager(new InputTest());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
