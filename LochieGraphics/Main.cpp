//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "WeatherScene.h"

int main()
{
	SceneManager sceneManager(new WeatherScene());
	while (!sceneManager.ShouldClose())
	{
		sceneManager.Update();
	}
}
