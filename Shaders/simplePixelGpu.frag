#version 460 core

out vec4 FragColor;

in vec2 texCoords;

//! #include "gpuPixelRep.glsl"

// TODO: Should these be constants instead
uniform int gridCols;
uniform int gridRows;


uniform int renderIndex;

// Lochie: TODO: Prob move this to a own file
const float PI = 3.1415926535897932384626433832795;

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
	
	Pixel pixel = grid[index];
	vec3 colour = vec3(0.0, 0.0, 0.0);

	if (renderIndex == 0) // Colour
	{
		colour = vec3(pixel.colour);
	}
	else if (renderIndex == 1) // Vel
	{
		colour = vec3(0.5 - (atan(pixel.vel.x) / PI), 0.5 - (atan(pixel.vel.y) / PI), 0.5);
	}
	else if (renderIndex == 2) // ID
	{
		uint gridValue = pixel.matID;
		if (gridValue == 1)
		{
			colour = vec3(0.62, 0.8, 0.84);
		}
		else if (gridValue == 2)
		{
			colour = vec3(1.0, 0.84, 0.37);
		}
		else {
			colour = vec3(1.0, 0.4 ,0.7);
		}
	}
	else if (renderIndex == 3) // subpos
	{
		colour = vec3(pixel.subPos.x + 0.5, pixel.subPos.y + 0.5, 0);
	}
	else // Invalid
	{
		colour = vec3(texCoords, 0.0);
	}

	FragColor = vec4(colour, 1.0);

	// FragColor = vec4(float(col) / gridCols, float(row) / gridRows, float(index) / (gridRows * gridCols), 1.0);
	// FragColor = vec4(texCoords, 0.0, 1.0);
}

