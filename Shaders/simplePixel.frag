#version 460 core

out vec4 FragColor;

in vec2 texCoords;

// TODO: Should these be constants instead
uniform int gridCols;
uniform int gridRows;

struct Pixel {
	uint colour;
};

layout(std430, binding = 4) buffer Pixels
{
	Pixel grid[];
};


void main()
{
	int col = int(gridCols * texCoords.x);
	int row = int(gridRows * texCoords.y);
	
	int index = col * gridRows + row;
	
	uint gridValue = grid[index].colour;
	vec3 pixelColour = vec3(
		gridValue >> 0 & 0xFF,
		gridValue >> 8 & 0xFF,
		gridValue >> 16 & 0xFF);

	FragColor = vec4(pixelColour / 255.0, 1.0);

	// FragColor = vec4(float(col) / gridCols, float(row) / gridRows, float(index) / (gridRows * gridCols), 1.0);
	// FragColor = vec4(texCoords, 0.0, 1.0);
}

