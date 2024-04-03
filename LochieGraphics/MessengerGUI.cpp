#include "MessengerGUI.h"

#include "MessengerServer.h"
#include "MessengerClient.h"

#include "imgui.h"


void MessengerGUI::Start()
{
	started = true;
	if (serverTicked) {
		messenger = new MessengerServer();
	}
	else {
		messenger = new MessengerClient();
	}
	messenger->Start(address, std::to_string(port));
}

// TODO: Really shouldn't be a gui function
void MessengerGUI::GUI()
{
	ImGui::Checkbox("Server?", &serverTicked);
	
	if (!serverTicked) {
		// TODO: check how to do with std::string, (hover over ImGui::InputText)
		ImGui::InputText("IPv4 address", address, maxAddressLength); // TODO: When ticked show ip
	}

	ImGui::InputInt("Port number", &port);

	if (ImGui::Button("Start!!!")) {
		Start();
	}

	ImGui::InputText("Message", messageBuffer, bufSize);

	if (ImGui::Button("Send!!!")) {
		messenger->Send(messageBuffer);
	}

	for (auto i = messenger->messages.rbegin(); i != messenger->messages.rend() ; i++)
	{
		ImGui::Text(i->c_str());
	}
}

void MessengerGUI::Update()
{
	if (!started) { return; }
	

	messenger->Update();

	//TODO: Handle disconnecting
}
