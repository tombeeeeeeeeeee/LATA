#pragma once

#include "Maths.h"
#include <vector>
#include "Graphics.h"
#include <string>

class LineRenderer
{
	struct Colour {
		float r;
		float g;
		float b;
	};

private:
	std::vector<glm::vec3> positions;
	std::vector<Colour> colours;
	bool initialised = false;

	Colour currentColour = { 1.0f,1.0f,1.0f };

	glm::vec3 firstPos;
	glm::vec3 lastPos;
	Colour firstColour = { 0.0f,0.0f,0.0f };
	Colour lastColour = { 0.0f,0.0f,0.0f };
	bool lineActive = false;

	GLuint positionBufferID = 0;
	GLuint colourBufferID = 0;

	GLuint VAO = 0;


public:
	LineRenderer() = default;
	~LineRenderer();
	LineRenderer(const LineRenderer&) = delete;
	LineRenderer(const LineRenderer&&) = delete;
	const LineRenderer& operator=(const LineRenderer&) = delete;
	const LineRenderer& operator=(LineRenderer&&) = delete;

	void Initialise();

	void SetColour(Colour colour);

	void DrawLineSegment(glm::vec3 start, glm::vec3 end);
	void DrawLineSegment(glm::vec3 start, glm::vec3 end, Colour colour);

	void FinishLineStrip();
	void FinishLineLoop();

	void AddPointToLine(glm::vec3 point);
	void AddPointToLine(glm::vec3 point, Colour colour);

	void UpdateFrame();

	void Clear();
	void Compile();
	void Draw();
};