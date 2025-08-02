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

void LineRenderer::DrawLineSegementXZ(glm::vec2 start, glm::vec2 end, float yOffset)
{
	glm::vec3 start3D = { start.x, yOffset, start.y };
	glm::vec3 end3D = { end.x, yOffset, end.y };
	DrawLineSegment(start3D, end3D);
}

void LineRenderer::DrawLineSegementXZ(glm::vec2 start, glm::vec2 end, Colour colour, float yOffset)
{
	glm::vec3 start3D = { start.x, yOffset, start.y };
	glm::vec3 end3D = { end.x, yOffset, end.y };
	DrawLineSegment(start3D, end3D, colour);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size, Plane plane)
{
	DrawCircle(centre, size, plane, currentColour);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size, Plane plane, Colour colour)
{
	DrawCircle(centre, size, plane, colour, GetCircleSegmentCount(size));
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
		std::cout << "ERROR: Position and colour buffer are different sizes in the line renderer which should not happen.\n";
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
	return 128;
	//return glm::clamp((int)(sqrtf(radius * 10.0f/1080.0f) * 32 + 4), 5, 128);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size, Plane plane, int segmentCount)
{
	DrawCircle(centre, size, plane, currentColour, segmentCount);
}

void LineRenderer::DrawCircle(glm::vec3 centre, float size, Plane plane, Colour colour, int segmentCount)
{
	float c = cos(2 * PI / segmentCount);
	float s = sin(2 * PI / segmentCount);

	glm::vec3 start;
	switch (plane)
	{
	case LineRenderer::Plane::XY:
		start = glm::vec3(0, size, 0);
		break;
	case LineRenderer::Plane::YZ:
		start = glm::vec3(0, 0, size);
		break;
	case LineRenderer::Plane::XZ:
		start = glm::vec3(0, 0, size);
		break;
	default:
		break;
	}
	glm::vec3 plotPoint(start);

	for (int i = 0; i < segmentCount - 1; i++)
	{
		positions.push_back(centre + plotPoint);
		switch (plane)
		{
		case LineRenderer::Plane::XY:
			plotPoint = { plotPoint.x * c - plotPoint.y * s, plotPoint.y * c + plotPoint.x * s, 0.0f };
			break;
		case LineRenderer::Plane::YZ:
			plotPoint = { 0.0f, plotPoint.y * c - plotPoint.z * s, plotPoint.z * c + plotPoint.y * s };
			break;
		case LineRenderer::Plane::XZ:
			plotPoint = { plotPoint.x * c - plotPoint.z * s, 0.0f, plotPoint.z * c + plotPoint.x * s };
			break;
		default:
			break;
		}
		positions.push_back(centre + plotPoint);
		colours.push_back(colour);
		colours.push_back(colour);
	}
	positions.push_back(centre + plotPoint);
	positions.push_back(centre + start);
	colours.push_back(colour);
	colours.push_back(colour);
}
