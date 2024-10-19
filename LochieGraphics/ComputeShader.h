#pragma once

#include "Shader.h"


class ComputeShader : public Shader
{
public:
	ComputeShader(std::string path);

	void Run(unsigned int xGroups, unsigned int yGroups, unsigned int zGroups);
};

