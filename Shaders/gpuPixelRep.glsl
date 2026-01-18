
// This needs to match the cpu representation

struct Pixel {
	int matID;
	bool movedLastUpdate;
	vec2 vel;
	vec2 subPos;
	unsigned int colour;
};
