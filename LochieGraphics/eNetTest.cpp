#include "eNetTest.h"

#include "EditorGUI.h"

#include <iostream>

void eNetTestScene::Start()
{
	if (enet_initialize() != 0) {
		std::cout << "Error, Enet failed to initialise \n";
		return;
	}
	inputKeyWatch = { GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D };
}

void eNetTestScene::Update(float delta)
{
	if (started) {
		
		
		Receive();

		if (glfwGetKeyScancode(GLFW_KEY_W) == GLFW_PRESS) {
			ENetPacket* packet = enet_packet_create(&newPlayerPacket, sizeof(newPlayerPacket), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
		}
	}
}

void eNetTestScene::OnKey(int key, int action)
{
	if (action != GLFW_PRESS) { return; }
	switch (key)
	{
	case GLFW_KEY_W:
		//ENetPacket* packet = enet_packet_create(&newPlayerPacket, sizeof(newPlayerPacket), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, enet_packet_create(&newPlayerPacket, sizeof(newPlayerPacket), ENET_PACKET_FLAG_RELIABLE));
		break;
	default:
		break;
	}
}


void eNetTestScene::StartServerOrClient()
{
	if (isServer) {
		std::cout << "Creating server...\n";
		address.host = ENET_HOST_ANY;
		address.port = 1234;

		host = enet_host_create(&address, 32, 2, 0, 0);
	}
	else {
		std::cout << "Attempting to connect client at " << ipToConnectTo << "...\n";

		host = enet_host_create(nullptr, 1, 2, 0, 0);
		address.port = 1234;

		enet_address_set_host(&address, ipToConnectTo.c_str());

		enet_host_connect(host, &address, 2, 5000);
	}
	started = true;
}

void eNetTestScene::Receive()
{
	ENetEvent receivedEvent;
	while (enet_host_service(host, &receivedEvent, 0) > 0)
	{
		std::cout << "Recieved a packet!\n";
		switch (receivedEvent.type)
		{
		case ENetEventType::ENET_EVENT_TYPE_CONNECT:
			std::cout << "Connected!\n";
			peer = receivedEvent.peer;
			break;
		case ENetEventType::ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "Disconnected!\n";
			break;
		case ENetEventType::ENET_EVENT_TYPE_RECEIVE:
			std::cout << "Received!\n";
			std::cout << "Received Data:" << receivedEvent.data << "\n" << receivedEvent.packet->data << "\n";

			break;
		case ENetEventType::ENET_EVENT_TYPE_NONE:
			std::cout << "Uh oh!\n";
			break;
		default:
			break;
		}
	}
}


eNetTestScene::~eNetTestScene()
{
	enet_deinitialize();
}

void eNetTestScene::GUI()
{
	ImGui::Begin("eNet Example");

	bool disabled = started;
	if (disabled) {
		ImGui::BeginDisabled();
	}

	ImGui::Checkbox("Is this the server?", &isServer);
	
	ImGui::InputText("Ip address to connect to", &ipToConnectTo);
	
	if (ImGui::Button("Start!")) {
		StartServerOrClient();
	}

	if (disabled) {
		ImGui::EndDisabled();


		ImGui::InputText("Text", &sendBuffer);

		if (ImGui::Button("SEND MESSAGE")) {
			ENetPacket* packet = enet_packet_create(&sendBuffer, sendBuffer.size(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
		}

		ImGui::DragInt("player id", &newPlayerPacket.index, 0.1f);
		ImGui::DragInt("enum thing (5)", &newPlayerPacket.temp);

		if (ImGui::DragFloat("X", &newPlayerPacket.posX, 0.001f)) {
			ENetPacket* packet = enet_packet_create(&newPlayerPacket, sizeof(newPlayerPacket), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
		}
		if (ImGui::DragFloat("Y", &newPlayerPacket.posY, 0.01f)) {
			ENetPacket* packet = enet_packet_create(&newPlayerPacket, sizeof(newPlayerPacket), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
		}
	}

	ImGui::End();
	
}