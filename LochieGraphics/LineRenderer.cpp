#include "LineRenderer.h"
#include <iostream>

void LineRenderer::Initialise()
{
	glGenBuffers(1, &positionBufferID);

	glGenBuffers(1, &colourBufferID);
	glGenVertexArrays(1, &VAO);


	initialised = true;
}
LineRenderer::~LineRenderer()
{
	if (initialised)
	{
		glDeleteBuffers(1, &positionBufferID);
		glDeleteBuffers(1, &colourBufferID);
	}	
}


void LineRenderer::SetColour(Colour colour)
{
	currentColour = colour;
}

void LineRenderer::DrawLineSegment(glm::vec3 start, glm::vec3 end)
{
	DrawLineSegment(start, end, currentColour);
}

void LineRenderer::DrawLineSegment(glm::vec3 start, glm::vec3 end, Colour colour)
{
	positions.push_back(start);
	positions.push_back(end);
	colours.push_back(colour);
	colours.push_back(colour);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size)
{
	DrawCircle(centre, size, currentColour);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size, Colour colour)
{
	DrawCircle(centre, size, GetCircleSegmentCount(size));
}

void LineRenderer::FinishLineStrip()
{
	if (lineActive)
	{
		lineActive = false;
	}
}

void LineRenderer::FinishLineLoop()
{
	if (lineActive)
	{
		positions.push_back(lastPos);
		positions.push_back(firstPos);
		colours.push_back(lastColour);
		colours.push_back(firstColour);
		lineActive = false;
	}
}

void LineRenderer::AddPointToLine(glm::vec3 point)
{
	AddPointToLine(point, currentColour);
}

void LineRenderer::AddPointToLine(glm::vec3 point, Colour colour)
{
	if (lineActive)
	{
		positions.push_back(lastPos);
		positions.push_back(point);
		colours.push_back(lastColour);
		colours.push_back(colour);
		lastPos = point;
		lastColour = colour;
	}
	else
	{
		lineActive = true;
		lastPos = point;
		firstPos = point;
		lastColour = colour;
		firstColour = colour;
	}
}

void LineRenderer::UpdateFrame()
{
	if (positions.size() != colours.size())
	{
		std::cout << "ERROR: Somehow, position and colour buffer are different sizes in the line renderer. This should never happen.\n";
		return;
	}
	if (positions.size() > 0)
	{
		Compile();
		Draw();
	}
	Clear();	//It's possible to have done a single point, in which case there's nothing to draw but we do have to set 'lineActive' to false.
}

void LineRenderer::Clear()
{
	lineActive = false;
	positions.clear();
	colours.clear();
}

void LineRenderer::Compile()
{
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colour) * colours.size(), colours.data(), GL_DYNAMIC_DRAW);

}

void LineRenderer::Draw()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glBindBuffer(GL_ARRAY_BUFFER, colourBufferID);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Colour), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINES, 0, (GLsizei)positions.size());
}

int LineRenderer::GetCircleSegmentCount(float radius) const
{
	return glm::clamp((int)(sqrtf(radius * 10.0f/1080.0f) * 32 + 4), 5, 128);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size, int segmentCount)
{
	DrawCircle(centre, size, currentColour, segmentCount);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size, Colour colour, int segmentCount)
{
	float c = cos(2 * PI / segmentCount);
	float s = sin(2 * PI / segmentCount);

	glm::vec3 plotPoint(0, 0, size);

	for (int i = 0; i < segmentCount - 1; i++)
	{
		positions.push_back(centre + plotPoint);
		plotPoint = { plotPoint.x * c - plotPoint.z * s, 0.0f, plotPoint.z * c + plotPoint.x * s };
		positions.push_back(centre + plotPoint);
		colours.push_back(colour);
		colours.push_back(colour);
	}
	positions.push_back(centre + plotPoint);
	positions.push_back(centre + glm::vec3(0, 0, size));
	colours.push_back(colour);
	colours.push_back(colour);
}
