//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "EmptyScene.h"
#include "ArtScene.h"
#include "InputTest.h"
#include "GameTest.h"
#include "LevelEditor.h"

int main()
{
	SceneManager sceneManager(new LevelEditor());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
