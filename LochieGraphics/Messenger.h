#pragma once

#include <vector>
#include <string>

class Messenger
{
public:
	static std::vector<std::string> messages;

	virtual void Start(std::string add, std::string port) = 0;
	virtual void Update() = 0;
	virtual void Close() = 0;

	virtual void Send(const char* message) = 0;
};
