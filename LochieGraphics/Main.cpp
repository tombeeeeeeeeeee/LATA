//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "EmptyScene.h"
#include "ArtScene.h"
#include "InputTest.h"




int main()
{
	SceneManager sceneManager(new TestScene());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
