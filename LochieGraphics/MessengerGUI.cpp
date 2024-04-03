#include "MessengerGUI.h"

#include "MessengerServer.h"
#include "MessengerClient.h"

#include "imguiStuff.h"


void MessengerGUI::Start()
{
	// Get address as string
	std::string add = std::to_string(address[0]) + "." + std::to_string(address[1]) + "." + std::to_string(address[2]) + "." + std::to_string(address[3]);

	started = true;
	if (serverTicked) {
		messenger = new MessengerServer();
	}
	else {
		messenger = new MessengerClient();
	}
	messenger->Start(add, std::to_string(port));
}

// TODO: Really shouldn't be a gui function
void MessengerGUI::GUI()
{
	ImGui::Checkbox("Server?", &serverTicked);
	
	if (started) {
		ImGui::BeginDisabled();
	}
	ImGui::InputInt4("IPv4 address", address);
	std::string add = std::to_string(address[0]) + "." + std::to_string(address[1]) + "." + std::to_string(address[2]) + "." + std::to_string(address[3]);
	ImGui::Text(add.c_str());

	ImGui::InputInt("Port number", &port);

	// TODO: see if combining these ifs are okay, should leave early???
	if (ImGui::Button("Start!!!")) {
		Start();
	}
	if (started) {
		ImGui::EndDisabled();
	}



	if (!started) {
		ImGui::BeginDisabled();
	}
	ImGui::InputText("Message", messageBuffer, bufSize);

	if (ImGui::Button("Send!!!")) {
		messenger->Send(messageBuffer);
	}

	//TODO: iterate properly
	for (size_t i = 0; i < messenger->messages.size(); i++)
	{
		ImGui::Text(messenger->messages[i].c_str());
	}
	if (!started) {
		ImGui::EndDisabled();
	}
}

void MessengerGUI::Update()
{
	if (!started) { return; }
	

	messenger->Update();

	//TODO: Handle disconnecting
}
