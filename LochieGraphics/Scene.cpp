#include "Scene.h"

void Scene::DefaultGUI()
{

}

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
