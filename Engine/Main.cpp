//#include "Lophics.h"
#include "SceneManager.h"

#include "TestScene.h"
#include "EmptyScene.h"
#include "ArtScene.h"
#include "InputTest.h"
#include "GameTest.h"
#include "LevelEditor.h"
#include "LocWorkshop.h"
#include "GameSyncEcco.h"

int main()
{
#if defined ART_TOOL
	ArtScene scene;
#elif defined LEVEL_EDITOR
	LevelEditor scene;
#else
	GameSyncEcco scene;
#endif
	SceneManager sceneManager(&scene);
	sceneManager.Run();
}
