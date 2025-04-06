#include "PixelSimulation.h"

#include "EditorGUI.h"

#include "Graphics.h"

// TODO: remove
#include <iostream>

void Pixels::Simulation::Chunk::Update(Simulation& pixelStuff)
{
	// TODO: What are other potential ways can this be done, instead of having each one needing an 'updated'
	for (auto& r : cells)
	{
		for (auto& i : r)
		{
			i.updated = false;
		}
	}

	// For now, just don't update border pixels
	// The way they are updated should be more considered, however this currently gives the desiered behaviour the way I want
	// Main gravity (just consider down)
	unsigned int start = (pixelStuff.spreadTest) ? 0 : chunkWidth - 1;
	short sign = pixelStuff.spreadTest ? 1 : -1;
	for (unsigned int c = start; (pixelStuff.spreadTest) ? (c < chunkWidth) : (c > 0); c += sign)
	{
		// TODO: Want to do a similar thing for alternating this too
		for (unsigned int r = 0; r < chunkHeight - 0; r++)
		{
			Cell& curr = getLocal(c, r);
			if (curr.updated) { continue; }
			const auto& mat = pixelStuff.getMat(curr.materialID);
			if (mat.flags & MaterialFlags::neverUpdate) { continue; }
			int globalX = x * chunkWidth + c;
			int globalY = y * chunkHeight + r;
			if (mat.flags & MaterialFlags::gravity) { pixelStuff.Gravity(curr, mat, globalX, globalY); }
		}
	}
}

Pixels::Cell& Pixels::Simulation::Chunk::getLocal(int x, int y)
{
	return cells[x][y];
}

Pixels::Simulation::Chunk::Chunk(int _x, int _y) :
	x(_x),
	y(_y)
{
	// Set up pixel colour SSBO, this is how the pixel colours are stored on the GPU
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// TODO: This shouldn't be a "PixelData" or something, as we wouldn't need to upload everything to the GPU (Probably just the colour)
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Pixels::GpuCell) * Pixels::chunkWidth * Pixels::chunkHeight, GetGpuPixelToDrawFrom(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Pixels::Simulation::Chunk::SetDebugColours()
{
	for (int col = 0; col < chunkWidth; col++)
	{
		for (int row = 0; row < chunkHeight; row++)
		{
			Cell& pixel = getLocal(col, row);
			pixel.colour.x = (unsigned char)(((float)col / (chunkWidth - 1)) * 255);
			pixel.colour.y = (unsigned char)(((float)row / (chunkHeight - 1)) * 255);
			pixel.colour.z = 0_uc;
			//pixel.colour = { 25uc, 25uc, 25uc, 25uc };
		}
	}
	// Setting specific pixels colour for reference
	getLocal(0, 0).colour = { 255_uc, 255_uc, 255_uc }; // TODO: Do we want 0,0 do be considered left top or bottom, currently bottom left
	getLocal(chunkWidth - 1, 0).colour = { 126_uc, 126_uc, 126_uc };
	getLocal(chunkWidth - 1, chunkHeight - 1).colour = { 0_uc, 0_uc, 0_uc };
}

void Pixels::Simulation::Chunk::PrepareDraw()
{
	for (int c = 0; c < chunkWidth; c++)
	{
		for (int r = 0; r < chunkHeight; r++)
		{
			const Cell& cell = getLocal(c, r);
			if (!drawVelocity) {
				GpuCells[c][r].colour = glm::u8vec4(cell.colour, 255_uc);
			}
			else {
				glm::u8vec4 colour;
				if (cell.velocity == glm::vec2(0, 0)) {
					colour = { 255_uc, 255_uc, 255_uc, 255_uc };
				}
				else {
					glm::vec2 dir = glm::normalize(cell.velocity);
					float angle = std::atan2f(dir.y, dir.x);
					float r, g, b;
					ImGui::ColorConvertHSVtoRGB(angle / (2 * PI) + 0.5f, 1.0f, 1.0f, r, g, b);
					colour = glm::u8vec4(r * 255, g * 255, b * 255, 1.0);
				}
				GpuCells[c][r].colour = colour;
			}
		}
	}
	// Update SSBO, the pixel colour data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	unsigned long long ssboSize = sizeof(Pixels::GpuCell) * Pixels::chunkWidth * Pixels::chunkHeight;
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ssboSize, GetGpuPixelToDrawFrom());
}

const Pixels::GpuCell* Pixels::Simulation::Chunk::GetGpuPixelToDrawFrom() const
{
	return &GpuCells[0][0];
}

Pixels::Simulation::Chunk& Pixels::Simulation::AddChunk(int x, int y)
{
	auto search = chunkLookup.find(std::pair<int, int>(x, y));
	if (search != chunkLookup.end()) {
		return *search->second;
	}
	if (chunks.size() >= maxChunks) {
		// TODO: Don't
		std::cout << "ERORORAIOSMDADASDDGVBDAS\n";
		throw;
	}
	Chunk& chunk = chunks.emplace_back(x, y);
	chunkLookup.emplace(std::pair<int, int>(x, y), &chunk);
	return chunk;
}

Pixels::Cell& Pixels::Simulation::getGlobal(int cellX, int cellY)
{
	int x = floorf((float)cellX / chunkWidth);
	int y = floorf((float)cellY / chunkHeight);

	auto search = chunkLookup.find(std::pair<int, int>(x, y));
	Chunk* chunk = nullptr;
	if (search == chunkLookup.end()) {
		if (chunks.size() >= maxChunks) {
			return theEdge;
		}
		else {
			chunk = &AddChunk(x, y);
		}
	}
	else {
		chunk = search->second;
	}
	return chunk->getLocal(cellX - (x * chunkWidth), cellY - (y * chunkHeight));
}

bool Pixels::Simulation::getSpread() const
{
	return spreadTest;
}

bool Pixels::Simulation::MovePixelToward(Cell& a, glm::ivec2 pos, glm::ivec2 desiredPos)
{
	auto path = GeneratePathFromToward(a, pos, desiredPos);
	bool previousSpace = false;
	const Material& mat = getMat(a.materialID);
	size_t i = 0;
	// TODO: Why am I even iterating this, it should be the last one as the path is now generated toward, instead as a whole
	for (; i < path.size(); i++)
	{
		if (pos == path[i]) {
			continue;
		}
		const Material& checkingMat = getMat(getGlobal(path[i].x, path[i].y).materialID);
		// TODO: Could edit this part to make materials flow through other materials slower
		// TODO: If we are keeping the density thing, should be a function for this check
		// TODO: If this is different materials each check could look quite weird, would need to change how the swapping or check works
		// TODO: This isn't even supposed to be here like this is it? I put it in the Generate Path From Toward
		if (checkingMat.density < mat.density) {
			previousSpace = true;
		}
		else if (previousSpace || (checkingMat.density == mat.density)) {
			break;
		}
	}
	if (previousSpace) {
		glm::ivec2 to = path[i - 1];
		// TODO: Change how this works
		if (i != path.size()) {
			//a.velocity *= 0.9f;
		}
		SwapPixels(a, getGlobal(to.x, to.y));
		return true;
	}
	return false;
}

void Pixels::Simulation::setCell(int x, int y, MatID matID)
{
	Cell& cell = getGlobal(x, y);
	if (cell.materialID == theEdge.materialID) {
		// TODO: Maybe a warning or something
		return;
	}
	const Material& mat = getMat(matID);
	cell.materialID = matID;
	cell.colour = mat.defaultColour;
	cell.velocity = { 0.0f, 0.0f };
	cell.updated = true;
}

bool Pixels::Simulation::SwapPixels(Cell& a, Cell& b)
{
	// TODO: Check for being the same pixel swapping with itself somehow?
	// TODO: this maybe shouldn't be here? Maybe a lil warning that this has happened?
	if (a.updated || b.updated) {
		return false;
	}

	// TODO: Should be creating a temp Pixeldata instead
	auto tempID = a.materialID;
	a.materialID = b.materialID;
	b.materialID = tempID;

	auto tempColour = a.colour;
	a.colour = b.colour;
	b.colour = tempColour;

	auto tempVel = a.velocity;
	a.velocity = b.velocity;
	b.velocity = tempVel;


	// neverUpdate pixels should not be marked as updated, they can be 'moved' around multiple times
	// Other types of pixels should be marked as updated if they are moved
	a.updated = getMat(a.materialID).flags != MaterialFlags::neverUpdate;
	b.updated = getMat(b.materialID).flags != MaterialFlags::neverUpdate;

	return true;
}

void Pixels::Simulation::Gravity(Cell& pixel, const Material& mat, int x, int y)
{
	if (testCenterGravity) {
		glm::vec2 pos = { x, y };
		glm::vec2 testPos = { chunkWidth, chunkHeight };
		float length = glm::length(testPos - pos);
		glm::vec2 add = glm::normalize(testPos - pos) * 0.1f;
		// TODO: is there something better then this
		if (!glm::isnan(add.x) && !glm::isnan(add.y)) {
			pixel.velocity += add;
		}
	}
	else {
		pixel.velocity += gravityForce;
	}
	if (glm::isnan(pixel.velocity.x)) {
		do {} while (true);
	}

	glm::ivec2 desiredPos = { floorf(x + pixel.velocity.x + 0.5f), floorf(y + pixel.velocity.y + 0.5f) };
	//desiredPos = { glm::clamp(desiredPos.x, 0, PIXELS_W - 1), glm::clamp(desiredPos.y, 0, PIXELS_H - 1) };
	//if (desiredPos.x < 0 || desiredPos.x >= PIXELS_W || desiredPos.y < 0 || desiredPos.y >= PIXELS_H) {
	//	return;
	//}
	// TODO: This function should be slightly different, consider if 2 pixels were clashing (on the x axis) and both pointed a lil up, they would be stuck
	bool moved = MovePixelToward(pixel, { x, y }, desiredPos);
	if (!moved) {
		//pixel.velocity *= 0.9f;
	}
	//auto& next = chunk.pixels[desiredPos.x][desiredPos.y];
	//const auto& nextMat = getMat(next.materialID);
	//if (nextMat.density < mat.density) {
	//	SwapPixels(pixel, next);
	//	return;
	//}
	//bool spread = true;
	//if (GravityDiagonalHelper(pixel, mat, c, r, spread)) { return; }
	//if (GravityDiagonalHelper(pixel, mat, c, r, !spread)) { return; }
}

bool Pixels::Simulation::GravityDiagonalHelper(Cell& pixel, const Material& mat, unsigned int c, unsigned int r, bool spread)
{
	auto nextC = c + ((spread) ? 1 : -1);
	auto& downNext = getGlobal(nextC, r -1 );
	const auto& downNextMat = getMat(downNext.materialID);
	if (downNextMat.density < mat.density) {
		if (SwapPixels(pixel, downNext)) {
			return true;
		}
	}
	return false;
}

const Pixels::Material& Pixels::Simulation::getMat(MatID index) const
{
	// TODO: Should there be a debug / not found material info
	if (index > materialInfos.size()) {
		return materialInfos.front();
	}
	return materialInfos[index];
}

Pixels::Material& Pixels::Simulation::getNonConstMat(MatID index)
{
	// TODO: Should there be a debug / not found material info
	if (index > materialInfos.size()) {
		return materialInfos.front();
	}
	return materialInfos[index];
}

std::vector<glm::ivec2> Pixels::Simulation::GeneratePathFromToward(const Cell& a, glm::ivec2 start, glm::ivec2 end)
{
	// Always contain the starting position
	std::vector<glm::ivec2> values = { start };
	if (start == end) {
		// Early return if the pixel locations are the same
		return values;
	}
	glm::vec2 checking = start;
	const float checkEvery = 0.9f;
	const glm::vec2 normal(glm::normalize(glm::vec2(end - start)));
	const float checkDistance = glm::length(glm::vec2(end - start)) - 0.5f;
	const glm::vec2 offset = normal * checkEvery;
	bool last = false;
	const glm::vec2 startF = glm::vec2(start);
	const auto& aMat = getMat(a.materialID);
	for (glm::vec2 checking = startF + offset; !last; checking += offset)
	{

		if (glm::length(checking - startF) >= checkDistance) {
			last = true;
			checking = end;
		}

		const glm::ivec2 checkingPixelPos(roundf(checking.x), roundf(checking.y));

		const auto& checkingMat = getMat(getGlobal(checkingPixelPos.x, checkingPixelPos.y).materialID);
		if (checkingMat.density >= aMat.density) {
			break;
		}

		// Don't insert value if it is already in
		if (checkingPixelPos != values.back()) {
			values.push_back(checkingPixelPos);
		}
	}
	return values;
}

void Pixels::Simulation::SetSimpleMaterials()
{
	// Would later load and save these to a file so that new materials could be made whenever and don't require a whole rebuild
	materialInfos = std::vector<Material>{
		{ "air", { 197_uc, 222_uc, 227_uc }, 0.0f, MaterialFlags::neverUpdate},
		{ "edge", { 255_uc, 0_uc, 255_uc }, FLT_MAX, MaterialFlags::neverUpdate },
		{ "sand", { 212_uc, 178_uc, 57_uc }, 1.0f, MaterialFlags::gravity},
		{ "stone", { 25_uc, 25_uc, 25_uc }, 10.0f, MaterialFlags::neverUpdate}
	};
}

void Pixels::Simulation::PrepareDraw(int left, int down)
{
	for (auto& i : chunkLookup)
	{
		if (i.second->draw) {
			i.second->PrepareDraw();
		}
	}
}

void Pixels::Simulation::Update()
{
	spreadTest = !spreadTest;
	
	int existingChunks = chunks.size();

	for (int i = 0; i < existingChunks; i++)
	{
		chunks[i].Update(*this);
	}
}

unsigned int Pixels::Simulation::AmountOf(MatID materialID) const
{
	unsigned int amount = 0;
	for (const auto& chunk : chunks)
	{
		for (const auto& c : chunk.cells)
		{
			for (const auto& i : c)
			{
				if (i.materialID == materialID) { amount++; }
			}
		}
	}
	return amount;
}

void Pixels::Simulation::SetCircleToMaterial(int cx, int cy, float radius, MatID materialID)
{
	for (int x = cx - radius; x < cx + radius + 1.0f; x++)
	{
		for (int y = cy - radius; y < cy + radius + 1.0f; y++) {
			if (sqrt(pow((float)x - cx, 2) + pow((float)y - cy, 2)) <= radius) {
				setCell(x, y, materialID);
			}
		}
	}
}

void Pixels::Simulation::SetEverythingTo(MatID materialID)
{
	for (auto& chunk : chunks)
	{
		for (auto& r : chunk.cells) {
			for (auto& i : r)
			{
				i.materialID = materialID;
			}
		}
	}
}

void Pixels::Simulation::SetEverythingToColour(const glm::vec3& colour)
{
	for (auto& chunk : chunks)
	{
		for (auto& r : chunk.cells) {
			for (auto& i : r)
			{
				i.colour = glm::vec3(colour * 255.0f);
			}
		}
	}
}

void Pixels::Simulation::SetAllToDefaultColour()
{
	for (auto& chunk : chunks)
	{
		for (auto& r : chunk.cells) {
			for (auto& i : r)
			{
				i.colour = getMat(i.materialID).defaultColour;
			}
		}
	}
}

void Pixels::Simulation::AddVelocityToCircle(int x, int y, float radius, glm::vec2 vel)
{
	for (int c = x - radius; c < x + radius + 1.0f; c++)
	{
		for (int r = y - radius; r < y + radius + 1.0f; r++) {
			if (sqrt(pow((float)c - x, 2) + pow((float)r - y, 2)) <= radius) {
				getGlobal(c, r).velocity += vel;
			}
		}
	}
}

void Pixels::Simulation::PixelGUI(int x, int y)
{
	auto& currPixel = getGlobal(x, y);

	currPixel.GUI();

	ImGui::SeparatorText("Pixel Material Info");
	getNonConstMat(currPixel.materialID).GUI();
}

std::vector<glm::ivec2> Pixels::GeneratePathBetween(glm::ivec2 start, glm::ivec2 end)
{
	// Always contain the starting position
	std::vector<glm::ivec2> values = { start };
	if (start == end) {
		// Early return if the pixel locations are the same
		return values;
	}
	glm::vec2 checking = start;
	const float checkEvery = 0.5f;
	const glm::vec2 normal = glm::normalize(glm::vec2(end - start));
	const float checkDistance = glm::length(glm::vec2(end - start)) - 0.5f;
	const glm::vec2 offset = normal * checkEvery;
	bool last = false;
	const glm::vec2 startF = glm::vec2(start);
	for (glm::vec2 checking = startF + offset; !last; checking += offset)
	{
		if (glm::length(checking - startF) >= checkDistance) {
			last = true;
			checking = end;
		}
		glm::ivec2 checkingPixelPos = glm::ivec2(roundf(checking.x), roundf(checking.y));
		// Don't insert value if it is already in
		if (checkingPixelPos != values.back()) {
			values.push_back(checkingPixelPos);
		}
	}
	return values;
}

bool Pixels::Simulation::isCellAt(int x, int y)
{
	return (chunkLookup.find(std::pair<int, int>(floorf((float)x / chunkWidth), floorf((float)y / chunkHeight))) != chunkLookup.end());
}

int Pixels::Simulation::getChunkCount()
{
	return chunks.size();
}

const std::vector<Pixels::Simulation::Chunk>& Pixels::Simulation::getChunks() const
{
	return chunks;
}

void Pixels::Simulation::SetDebugColours()
{
	for (auto& chunk : chunks)
	{
		chunk.SetDebugColours();
	}
}
void Pixels::Simulation::SetDrawVelocity(bool value)
{
	for (auto& chunk : chunks)
	{
		chunk.drawVelocity = value;
	}
}

Pixels::Simulation::Simulation()
{
	chunks.reserve(maxChunks);
	theEdge.materialID = 1;
}

