#pragma once

#include "Shader.h"


class ComputeShader : public Shader
{
private:
	ComputeShader() = default;

public:
	ComputeShader(std::string path);

	static ComputeShader* CreateCustomComputeShader(std::string code);

	void Run(unsigned int xGroups, unsigned int yGroups, unsigned int zGroups, unsigned int barrier);
};

