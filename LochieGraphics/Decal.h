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
	float depthOfDecal;
	float scale;

	Material* mat;

	Decal() {};
	Decal(toml::table table);
	void GUI();
	toml::table Serialise(unsigned long long guid);
};