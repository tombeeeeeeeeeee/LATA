#pragma once

namespace toml {
	inline namespace v3 {
		class table;
	}
}

struct ShadowWall
{

	bool hasLocalMesh = false; 
	void GUI();
	ShadowWall() {};
	ShadowWall(toml::table table);

	toml::table Serialise(unsigned long long guid);
};