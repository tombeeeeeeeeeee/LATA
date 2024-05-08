#pragma once

#include "Scene.h"

#include "enet/enet.h"

#include <string>

class eNetTestScene : public Scene
{
public:
	void Start() override;

	void Update(float delta) override;

	void OnKey(int key, int action) override;
	

	~eNetTestScene();

	void GUI() override;


private:
	void StartServerOrClient();

	void Receive();


	std::string ipToConnectTo;

	bool started = false;
	bool isServer = false;

	std::string sendBuffer = "";

	// TODO: Might not need to keep this
	ENetAddress address;
	ENetHost* host;


	struct NewPlayerPacket {
		int index = 1;
		int temp = 5;
		float posX;
		float posY;
	};
	NewPlayerPacket newPlayerPacket;

	ENetPeer* peer;

};