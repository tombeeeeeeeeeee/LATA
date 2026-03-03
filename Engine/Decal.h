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

	Material* mat = nullptr;

	Decal() {};
	Decal(toml::table table);
	static Decal* Load(toml::table table);
	void PartGUI();
	toml::table Serialise(unsigned long long guid);
};