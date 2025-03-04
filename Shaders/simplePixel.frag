#version 460 core

out vec4 FragColor;

in vec2 texCoords;

uniform int gridCols;
uniform int gridRows;

struct Pixel {
	vec4 colour;
};

layout(std430, binding = 4) buffer Pixels
{
    Pixel grid[];
};



void main()
{
	int col = int(gridCols * texCoords.x);
	int row = int(gridRows * texCoords.y);
	
	int index = row * gridRows + col;
	
	FragColor = vec4(grid[index].colour.rgb, 1.0);

	// FragColor = vec4((float(index) / (gridRows * gridCols)).xxx, 1.0);

	//	FragColor = vec4(float(col) / gridCols, float(row) / gridRows, float(index) / (gridRows * gridCols), 1.0);

	//FragColor = vec4(texCoords, 0.0, 1.0);


}