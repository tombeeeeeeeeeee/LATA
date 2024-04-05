#include "Scene.h"

Scene::Scene()
{
	gui.scene = this;
}

Scene::~Scene()
{
	while (!sceneObjects.empty())
	{
		delete sceneObjects.back();
		sceneObjects.pop_back();
	}
}
