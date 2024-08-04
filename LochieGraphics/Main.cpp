//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "EmptyScene.h"
#include "ArtScene.h"



int main()
{
	SceneManager sceneManager(new EmptyScene());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
