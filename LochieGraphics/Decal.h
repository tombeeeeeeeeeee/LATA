#pragma once

class Material;

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Decal
{
public:
	float depthOfDecal = 0.002f;
	float angleTolerance = 1.0f;
	float scale = 1.0f;

	Material* mat;

	Decal() {};
	Decal(toml::table table);
	void GUI();
	toml::table Serialise(unsigned long long guid);
};