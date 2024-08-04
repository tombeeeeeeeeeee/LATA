//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "EmptyScene.h"
#include "ArtScene.h"
#include "InputTest.h"
#include "GameTest.h"

int main()
{
	SceneManager sceneManager(new GameTest());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
