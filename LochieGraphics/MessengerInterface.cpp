#include "MessengerInterface.h"

#include "MessengerServer.h"
#include "MessengerClient.h"

#include "imgui.h"


void MessengerInterface::Start()
{
	if (serverTicked) {
		messenger = new MessengerServer();
	}
	else {
		messenger = new MessengerClient();
	}
	started = messenger->Start(address, std::to_string(port));
	if (!started) {
		std::cout << "Error: Failed to start messenger\n";
		messenger->Close();
		delete messenger;
		messenger = nullptr;
	}
}

// TODO: Really shouldn't be a gui function
void MessengerInterface::GUI()
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

	ImGui::InputText("Message", typedMessage, maxMessageLength);

	if (ImGui::Button("Send!!!")) {
		messenger->Send(typedMessage);
	}

	for (auto i = messenger->messages.rbegin(); i != messenger->messages.rend() ; i++)
	{
		ImGui::Text(i->c_str());
	}
}

void MessengerInterface::Update()
{
	if (!started) { return; }
	

	messenger->Update();

	//TODO: Handle disconnecting
}
