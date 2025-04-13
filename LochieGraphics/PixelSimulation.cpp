#include "PixelSimulation.h"

#include "EditorGUI.h"

#include "Graphics.h"

// TODO: remove
#include <iostream>

unsigned char Pixels::Simulation::updateOrder = 0;
bool Pixels::Simulation::leftToRight = false;
bool Pixels::Simulation::upToDown = false;

void Pixels::Simulation::Chunk::Update(Simulation& sim)
{
	if (!prevUpdated) {
		return;
	}
	int cStart = ((sim.leftToRight) ? 0 : chunkWidth - 1);
	signed char cSign = (sim.leftToRight ? 1 : -1);

	int rStart = ((sim.upToDown) ? chunkHeight - 1 : 0);
	signed char rSign = ((sim.upToDown) ? -1 : 1);

	auto cCheck = (sim.leftToRight ? 
		([](signed int a) -> bool { return a < chunkWidth; }) :
		([](signed int a) -> bool { return a >= 0; }));

	auto rCheck = (sim.upToDown ?
		([](signed int a) -> bool { return a >= 0; }) :
		([](signed int a) -> bool { return a < chunkHeight; }));

	for (signed int c = cStart; cCheck(c); c += cSign)
	{
		for (signed int r = rStart; rCheck(r); r += rSign)
		{
			Cell& curr = getLocal(c, r);
			if (curr.updated) { continue; }
			const auto& mat = sim.getMat(curr.materialID);
			if (mat.flags & MaterialFlags::neverUpdate) { continue; }
			int globalX = x * chunkWidth + c;
			int globalY = y * chunkHeight + r;
			if (mat.flags & MaterialFlags::gravity) {
				updated = true;
				sim.Gravity(curr, mat, globalX, globalY); 
			}
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
	if (!prevUpdated) {
		return;
	}
	if (!ssboGenerated) {
		ssboGenerated = true;
		// Set up pixel colour SSBO, this is how the pixel colours are stored on the GPU
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		// TODO: This shouldn't be a "PixelData" or something, as we wouldn't need to upload everything to the GPU (Probably just the colour)
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Pixels::GpuCell) * Pixels::chunkWidth * Pixels::chunkHeight, GetGpuPixelToDrawFrom(), GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
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
					float length = glm::length(cell.velocity);
					float angle = std::atan2f(dir.y, dir.x);
					float r, g, b;
					ImGui::ColorConvertHSVtoRGB(angle / (2 * PI) + 0.5f, fminf(length / 10.0f, 1.0f), 1.0f, r, g, b);
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
	Chunk* chunk = getChunkNonConst(cellX, cellY);
	if (chunk == nullptr) {
		return theEdge;
	}
	return chunk->getLocal(cellX - (chunk->x * chunkWidth), cellY - (chunk->y * chunkHeight));
}

bool Pixels::Simulation::MovePixelToward(Cell& a, glm::ivec2 pos, glm::ivec2 desiredPos)
{
	bool returnValue = false;
	auto path = GeneratePathFromToward(a, pos, desiredPos);
	if (path.size() > 1) {
		glm::ivec2 to = path.back();
		SwapPixels(a, getGlobal(to.x, to.y));
		returnValue = true;
	}
	glm::vec2 hitPos = glm::vec2(path.back()) + glm::normalize(a.velocity);
	glm::ivec2 hitCellPos(roundf(hitPos.x), roundf(hitPos.y));
	if (hitCellPos == path.back()) {
		std::cout << "Error\n";
	}
	Cell& hitCell = getGlobal(hitCellPos.x, hitCellPos.y);
	hitCell.colour.y += 1_uc;

	glm::vec2 relativeVel = hitCell.velocity - a.velocity;
	glm::vec2 normal = glm::normalize(glm::vec2(hitCellPos) - glm::vec2(path.back()));
	if (glm::dot(normal, relativeVel) >= 0) {
		return returnValue;
	}

	const Material& aMat = getMat(a.materialID);
	const Material& hMat = getMat(hitCell.materialID);
	if (hMat.flags & MaterialFlags::neverUpdate) {
		return returnValue;
	}
	float combinedInverseMass = (1 / aMat.density) + (1 / hMat.density);
	float elasticity = 1.0f;
	glm::vec2 j = (-(1 + elasticity) * glm::dot(relativeVel, normal) / combinedInverseMass) * normal;
	a.velocity += -j * 1.0f / aMat.density;
	hitCell.velocity += j * 1.0f / hMat.density;
	return returnValue;
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

void Pixels::Simulation::UpdateChunks(glm::ivec2 check, const std::vector<Chunk*>& updateChunks)
{
	for (int i = 0; i < updateChunks.size(); i++)
	{
		Simulation& ref = *this;
		Chunk* chunk = updateChunks.at(i);
		if (abs(chunk->x % 2) == check.x && abs(chunk->y % 2) == check.y) {
			threadPool.DoJob(std::bind(UpdateChunk, chunk, std::ref(ref)));
		}
	}
	while (threadPool.getJobsWaiting() > 0)
	{
		// Waiting for threads
	}
}

void Pixels::Simulation::UpdateChunk(Chunk* chunk, Simulation& sim)
{
	chunk->Update(sim);
}

void Pixels::Simulation::Gravity(Cell& pixel, const Material& mat, int x, int y)
{
	if (testCenterGravity) {
		glm::vec2 pos = { x, y };
		glm::vec2 testPos = { 0.0f, 0.0f };
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

	// TODO: Just use round instead of floor+.5
	glm::ivec2 desiredPos = { floorf(x + pixel.velocity.x + 0.5f), floorf(y + pixel.velocity.y + 0.5f) };
	//desiredPos = { glm::clamp(desiredPos.x, 0, PIXELS_W - 1), glm::clamp(desiredPos.y, 0, PIXELS_H - 1) };
	//if (desiredPos.x < 0 || desiredPos.x >= PIXELS_W || desiredPos.y < 0 || desiredPos.y >= PIXELS_H) {
	//	return;
	//}
	// TODO: This function should be slightly different, consider if 2 pixels were clashing (on the x axis) and both pointed a lil up, they would be stuck
	bool moved = MovePixelToward(pixel, { x, y }, desiredPos);
	glm::vec2 pos(x, y);
	glm::vec2 originalDesPos(x + pixel.velocity.x, y + pixel.velocity.y);
	glm::vec2 originalOffset = originalDesPos - pos;
	glm::vec2 direction = glm::normalize(originalOffset);
	if (glm::isnan(direction.x)) {
		return;
	}
	float length = glm::length(originalOffset);
	float originalDesAngle = std::atan2f(direction.y, direction.x);
	if (!moved) {
		float angle = originalDesAngle + ((leftToRight ? -1 : 1) * mat.halfAngleSpread);
		glm::vec2 desiredNormal(cosf(angle), sinf(angle));
		glm::vec2 newDesiredPos = pos + (desiredNormal * length);
		desiredPos = { floorf(newDesiredPos.x + 0.5f), floorf(newDesiredPos.y + 0.5f) };
		moved = MovePixelToward(pixel, { x, y }, desiredPos);
	}
	if (!moved) {
		float angle = originalDesAngle + ((leftToRight ? 1 : -1) * mat.halfAngleSpread);
		glm::vec2 desiredNormal(cosf(angle), sinf(angle));
		glm::vec2 newDesiredPos = pos + (desiredNormal * length);
		desiredPos = { floorf(newDesiredPos.x + 0.5f), floorf(newDesiredPos.y + 0.5f) };
		moved = MovePixelToward(pixel, { x, y }, desiredPos);
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
	float checkDistance = glm::length(glm::vec2(end - start)) - 0.5f;
	if (checkDistance > maxTravelDistance) {
		// Minus 0.5f to ensure rounding doesn't cause distance past maxTravelDistance
		glm::vec2 temp = normal * (maxTravelDistance - 0.5f);
		end = start + glm::ivec2(roundf(temp.x), roundf(temp.y));
		checkDistance = glm::length(glm::vec2(end - start)) - 0.5f;
	}
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
		{ "stone", { 25_uc, 25_uc, 25_uc }, 10.0f, MaterialFlags::neverUpdate},
		{ "water", { 100_uc, 170_uc, 255_uc }, 0.5f, MaterialFlags::gravity},
	};
	materialInfos[4].halfAngleSpread = PI / 2;
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
	updateOrder++;
	updateOrder %= 4;
	
	leftToRight = updateOrder % 2 == 0;
	upToDown = updateOrder < 2;
	
	std::vector<Chunk*> updateChunks(chunks.size());
	for (size_t i = 0; i < chunks.size(); i++)
	{
		updateChunks[i] = &chunks[i];
	}

	std::qsort(updateChunks.data(), updateChunks.size(), sizeof(Chunk*), ChunkSort);

	for (int i = 0; i < updateChunks.size(); i++)
	{
		// TODO: What are other potential ways can this be done, instead of having each one needing an 'updated'
		for (auto& r : updateChunks.at(i)->cells)
		{
			for (auto& i : r)
			{
				i.updated = false;
			}
		}
	}

	if (multithreaded) {
		UpdateChunks({ 0, 0 }, updateChunks);
		UpdateChunks({ 0, 1 }, updateChunks);
		UpdateChunks({ 1, 0 }, updateChunks);
		UpdateChunks({ 1, 1 }, updateChunks);
	}
	else {
		for (size_t i = 0; i < updateChunks.size(); i++)
		{
			updateChunks.at(i)->Update(*this);
		}
	}
	for (size_t i = 0; i < updateChunks.size(); i++)
	{
		Chunk* chunk = updateChunks.at(i);
		chunk->prevUpdated = chunk->updated;
		chunk->updated = false;
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

void Pixels::Simulation::PixelGUI(glm::ivec2 xy)
{
	PixelGUI(xy.x, xy.y);
}

void Pixels::Simulation::PixelGUI(int x, int y)
{
	auto& currPixel = getGlobal(x, y);

	currPixel.GUI();

	ImGui::SeparatorText("Pixel Material Info");
	getNonConstMat(currPixel.materialID).GUI();
}

bool Pixels::Simulation::isCellAt(int x, int y)
{
	return (chunkLookup.find(std::pair<int, int>(floorf((float)x / chunkWidth), floorf((float)y / chunkHeight))) != chunkLookup.end());
}

int Pixels::Simulation::getChunkCount() const
{
	return chunks.size();
}

const std::vector<Pixels::Simulation::Chunk>& Pixels::Simulation::getChunks() const
{
	return chunks;
}

const Pixels::Simulation::Chunk* Pixels::Simulation::getChunk(int cellX, int cellY) const
{
	int x = (int)floorf((float)cellX / chunkWidth);
	int y = (int)floorf((float)cellY / chunkHeight);

	auto search = chunkLookup.find(std::pair<int, int>(x, y));
	if (search == chunkLookup.end()) {
		return nullptr;
	}
	return search->second;
}

Pixels::Simulation::Chunk* Pixels::Simulation::getChunkNonConst(int cellX, int cellY)
{
	int x = (int)floorf((float)cellX / chunkWidth);
	int y = (int)floorf((float)cellY / chunkHeight);

	auto search = chunkLookup.find(std::pair<int, int>(x, y));
	Chunk* chunk = nullptr;
	if (search == chunkLookup.end()) {
		std::lock_guard<std::mutex> lock(chunkCreationLock);
		if (chunks.size() >= maxChunks) {
			return nullptr;
		}
		else {
			chunk = &AddChunk(x, y);
		}
	}
	else {
		chunk = search->second;
	}
	chunk->updated = true;
	return chunk;
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

Pixels::Simulation::Simulation() : threadPool(2)
{
	chunks.reserve(maxChunks);
	chunkLookup.reserve(maxChunks);
	theEdge.materialID = 1;
}

int Pixels::Simulation::ChunkSort(const void* l, const void* r)
{
	const Chunk* a = static_cast<const Chunk*>(l);
	const Chunk* b = static_cast<const Chunk*>(r);
	if (a->y < b->y) {
		return upToDown ? -1 : 1;
	}
	else if (b->y < a->y) {
		return upToDown ? 1 : -1;
	}

	if (a->x < b->x) {
		return leftToRight ? -1 : 1;
	}
	else if (b->x < a->x) {
		return leftToRight ? 1 : -1;
	}
	else {
		// Shouldn't be able to be here
		// TODO: error
		return 0;
	}
}

