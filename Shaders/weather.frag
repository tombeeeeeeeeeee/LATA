#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

const int rows = 20;
const int collumns = 20;

uniform vec2[rows * collumns] vectorMap;

int getIndex(int row, int col)
{
	return row * collumns + col;
}

int getRow(int index)
{
	return index / collumns;
}

int getCol(int index)
{
	return index % collumns;
}


void main()
{
    //vec3 colour = texture(image, TexCoord).rgb;
    
    
    vec3 colour;
    int index = getIndex(clamp(int(TexCoord.x * rows), 0, rows), clamp(int(TexCoord.y * collumns), 0, collumns));
    colour.x = vectorMap[index].x / 2 + 0.5;
    colour.y = vectorMap[index].y / 2 + 0.5;
    colour.z = -vectorMap[index].x / 2 + 0.5;
    
    //colour = vec3(1, 0, 1);
    
    FragColor = vec4(colour, 1.0);
}

