//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "EmptyScene.h"
#include "ArtScene.h"
#include "InputTest.h"
#include "GameTest.h"
#include "LevelEditor.h"
#include "LocWorkshop.h"

int main()
{
	LevelEditor scene;
	SceneManager sceneManager(&scene);
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
