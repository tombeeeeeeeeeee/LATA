#pragma once

namespace toml {
	inline namespace v3 {
		class table;
	}
}

struct ShadowWall
{

	bool hasLocalMesh = false; 
	void PartGUI();
	ShadowWall() {};
	ShadowWall(toml::table table);
	static ShadowWall* Load(toml::table table);

	toml::table Serialise(unsigned long long guid);
};