#version 460 core

out vec4 FragColor;

in vec2 texCoords;

// TODO: Should these be constants instead
uniform int gridCols;
uniform int gridRows;

//! #include "gpuPixelRep.glsl"

layout(std430, binding = 4) buffer Pixels
{
	Pixel grid[];
};

int indexFromCoords(ivec2 coord)
{
	return (coord.x % gridCols) + (coord.y * gridCols);
}

void main()
{
	int col = int(gridCols * texCoords.x);
	int row = int(gridRows * texCoords.y);
	
	int index = indexFromCoords(ivec2(col, row));
	
	uint gridValue = grid[index].matID;

	vec3 pixelColour = vec3(0.0, 0.0, 0.0);
	if (gridValue == 1)
	{
		pixelColour = vec3(0.62, 0.8, 0.84);
	}
	else if (gridValue == 2)
	{
		pixelColour = vec3(1.0, 0.84, 0.37);
	}

	FragColor = vec4(pixelColour, 1.0);

	// FragColor = vec4(float(col) / gridCols, float(row) / gridRows, float(index) / (gridRows * gridCols), 1.0);
	// FragColor = vec4(texCoords, 0.0, 1.0);
}

