#include "PixelStuff.h"

#include "ExtraEditorGUI.h"

#include "Utilities.h"


bool PixelStuff::MovePixelToward(PixelData& a, glm::ivec2 pos, glm::ivec2 desiredPos)
{
	auto path = GeneratePathFromToward(a, pos, desiredPos);
	bool previousSpace = false;
	const MaterialInfo& mat = getMat(a.materialID);
	size_t i = 0;
	for (; i < path.size(); i++)
	{
		if (pos == path[i]) {
			continue;
		}
		// TODO: Should be a better way to get pixel at location
		const MaterialInfo& checkingMat = getMat(chunk.pixels[path[i].x][path[i].y].materialID);
		// TODO: Could edit this part to make materials flow through other materials slower
		// TODO: If we are keeping the density thing, should be a function for this check
		// TODO: If this is different materials each check could look quite weird, would need to change how the swapping or check works
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
		SwapPixels(a, chunk.pixels[to.x][to.y]);
		return true;
	}
	return false;
}

bool PixelStuff::SwapPixels(PixelData& a, PixelData& b) {
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

PixelStuff::MaterialInfo& PixelStuff::getNonConstMat(MatID index)
{
	// TODO: Should there be a debug / not found material info
	if (index > materialInfos.size()) {
		return materialInfos.front();
	}
	return materialInfos[index];
}

const PixelStuff::GpuPixelData* PixelStuff::GetGpuPixelToDrawFrom() const
{
	return &GpuPixels[0][0];
}

bool PixelStuff::getSpread() const
{
	return spreadTest;
}

std::vector<glm::ivec2> PixelStuff::GeneratePathBetween(glm::ivec2 start, glm::ivec2 end)
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

std::vector<glm::ivec2> PixelStuff::GeneratePathFromToward(const PixelData& a, glm::ivec2 start, glm::ivec2 end)
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

		const auto& checkingMat = getMat(chunk.pixels[checkingPixelPos.x][checkingPixelPos.y].materialID);
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

const PixelStuff::MaterialInfo& PixelStuff::getMat(MatID index) const
{
	// TODO: Should there be a debug / not found material info
	if (index > materialInfos.size()) {
		return materialInfos.front();
	}
	return materialInfos[index];
}

bool PixelStuff::GravityDiagonalHelper(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r, bool spread) {
	auto nextC = c + ((spread) ? 1 : -1);
	if (nextC < 0 || nextC >= PIXELS_W) {
		return false;
	}
	auto& downNext = chunk.pixels[nextC][r - 1];
	const auto& downNextMat = getMat(downNext.materialID);
	if (downNextMat.density < mat.density) {
		if (SwapPixels(pixel, downNext)) {
			return true;
		}
	}
	return false;
}

void PixelStuff::SetCircleToMaterial(int x, int y, float radius, MatID materialID)
{
	for (size_t c = glm::max(0.0f, x - radius); c < glm::min(x + radius + 1.0f, (float)PIXELS_W); c++)
	{
		for (size_t r = glm::max(0.0f, y - radius); r < glm::min(y + radius + 1.0f, (float)PIXELS_H); r++) {
			if (sqrt(pow((float)c - x, 2) + pow((float)r - y, 2)) <= radius) {
				chunk.pixels[c][r].materialID = materialID;
			}
		}
	}
}

unsigned int PixelStuff::AmountOf(MatID materialID) const
{
	unsigned int amount = 0;
	for (const auto& c : chunk.pixels)
	{
		for (const auto& i : c)
		{
			if (i.materialID == materialID) { amount++; }
		}
	}
	return amount;
}

void PixelStuff::UpdateSim()
{
	spreadTest = !spreadTest;

	chunk.Update(*this);

	// TODO: Debug stuff organise
	for (auto& c : chunk.pixels) {
		for (auto& i : c)
		{
			i.colour = getMat(i.materialID).defaultColour;
		}
	}
}

void PixelStuff::PrepareDraw()
{
	for (size_t c = 0; c < PIXELS_W; c++)
	{
		for (size_t r = 0; r < PIXELS_H; r++)
		{
			GpuPixels[c][r].colour = glm::u8vec4(chunk.pixels[c][r].colour, 255_uc);
		}
	}
}


void PixelStuff::SetSimpleMaterials()
{
	// Would later load and save these to a file so that new materials could be made whenever and don't require a whole rebuild
	materialInfos = std::vector<MaterialInfo>{
		{ "air", { 197_uc, 222_uc, 227_uc }, 0.0f, MaterialFlags::neverUpdate},
		{ "sand", { 212_uc, 178_uc, 57_uc }, 1.0f, MaterialFlags::gravity},
		{ "stone", { 25_uc, 25_uc, 25_uc }, 10.0f, MaterialFlags::neverUpdate}
	};
}

void PixelStuff::SetDebugColours()
{
	for (size_t col = 0; col < PIXELS_W; col++)
	{
		for (size_t row = 0; row < PIXELS_H; row++)
		{
			PixelData& pixel = chunk.pixels[col][row];
			pixel.colour.x = (unsigned char)(((float)col / (PIXELS_W - 1)) * 255);
			pixel.colour.y = (unsigned char)(((float)row / (PIXELS_H - 1)) * 255);
			pixel.colour.z = 0_uc;
			//pixel.colour = { 25uc, 25uc, 25uc, 25uc };
		}
	}
	// Setting specific pixels colour for reference
	chunk.pixels[0][0].colour = { 255_uc, 255_uc, 255_uc }; // TODO: Do we want 0,0 do be considered left top or bottom, currently bottom left
	chunk.pixels[PIXELS_W - 1][0].colour = { 126_uc, 126_uc, 126_uc };
	chunk.pixels[PIXELS_W - 1][PIXELS_H - 1].colour = { 0_uc, 0_uc, 0_uc };

}

void PixelStuff::SetEverythingTo(MatID materialID)
{
	for (auto& r : chunk.pixels) {
		for (auto& i : r)
		{
			i.materialID = materialID;
		}
	}
}

void PixelStuff::SetEverythingToColour(const glm::vec3& colour)
{
	for (auto& r : chunk.pixels) {
		for (auto& i : r)
		{
			i.colour = glm::vec3(colour * 255.0f);
		}
	}
}

void PixelStuff::SetAllToDefaultColour()
{
	for (auto& r : chunk.pixels) {
		for (auto& i : r)
		{
			i.colour = getMat(i.materialID).defaultColour;
		}
	}
}

void PixelStuff::AddVelocityTo(int x, int y, float radius, glm::vec2 vel)
{
	for (size_t c = glm::max(0.0f, x - radius); c < glm::min(x + radius + 1.0f, (float)PIXELS_W); c++)
	{
		for (size_t r = glm::max(0.0f, y - radius); r < glm::min(y + radius + 1.0f, (float)PIXELS_H); r++) {
			if (sqrt(pow((float)c - x, 2) + pow((float)r - y, 2)) <= radius) {
				chunk.pixels[c][r].velocity += vel;
			}
		}
	}
}

void PixelStuff::PixelGUI(int x, int y)
{
	auto& currPixel = chunk.pixels[x][y];

	currPixel.GUI();

	ImGui::SeparatorText("Pixel Material Info");
	getNonConstMat(currPixel.materialID).GUI();
}

// TODO: Move function
void PixelStuff::Gravity(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r) {	
	if (testCenterGravity) {
		glm::vec2 pos = { c, r };
		glm::vec2 testPos = { PIXELS_W / 2, PIXELS_H / 2 };
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

	glm::ivec2 desiredPos = { c + pixel.velocity.x + 0.5f, r + pixel.velocity.y + 0.5f };
	desiredPos = { glm::clamp(desiredPos.x, 0, PIXELS_W - 1), glm::clamp(desiredPos.y, 0, PIXELS_H - 1) };
	//if (desiredPos.x < 0 || desiredPos.x >= PIXELS_W || desiredPos.y < 0 || desiredPos.y >= PIXELS_H) {
	//	return;
	//}
	// TODO: This function should be slightly different, consider if 2 pixels were clashing (on the x axis) and both pointed a lil up, they would be stuck
	bool moved = MovePixelToward(pixel, { c, r }, desiredPos);
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

PixelStuff::MaterialInfo::MaterialInfo(std::string _name, glm::u8vec3(_colour), float _density, unsigned int _flags) :
	name(_name),
	defaultColour(_colour),
	density(_density),
	flags(_flags)
{
}

void PixelStuff::MaterialInfo::GUI()
{
	std::string tag = Utilities::PointerToString(this);

	ImGui::InputText(("Name##" + tag).c_str(), &name);
	ExtraEditorGUI::ColourEdit3("Coloadsaur##" + tag, defaultColour);
	ImGui::CheckboxFlags(("Never Update##" + tag).c_str(), &flags, MaterialFlags::neverUpdate);
	ImGui::CheckboxFlags(("Gravity##" + tag).c_str(), &flags, MaterialFlags::gravity);
	ImGui::InputFloat(("Density##" + tag).c_str(), &density);
}

void PixelStuff::PixelData::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ExtraEditorGUI::ColourEdit3("Colour##" + tag, colour);
	ImGui::DragFloat2(("Velocity##" + tag).c_str(), &velocity.x);
	// TODO: Should be some better GUI option for this
	ImGui::InputScalar("Material ID", ImGuiDataType_U16, &materialID);
}

void PixelStuff::Chunk::Update(PixelStuff& pixelStuff)
{
	// TODO: What are other potential ways can this be done, instead of having each one needing an 'updated'
	for (auto& r : pixels)
	{
		for (auto& i : r)
		{
			i.updated = false;
		}
	}

	// For now, just don't update border pixels
	// The way they are updated should be more considered, however this currently gives the desiered behaviour the way I want
	// Main gravity (just consider down)
	unsigned int start = (pixelStuff.spreadTest) ? 1 : PIXELS_W - 1;
	short sign = pixelStuff.spreadTest ? 1 : -1;
	for (unsigned int c = start; (pixelStuff.spreadTest) ? (c < PIXELS_W - 1) : (c > 1); c += sign)
	//for (size_t c = 1; c < PIXELS_W - 1; c++)
	{
		// TODO: Want to do a similar thing for alternating this too
		for (unsigned int r = 1; r < PIXELS_H - 1; r++)
		{
			auto& curr = pixels[c][r];
			if (curr.updated) { continue; }
			const auto& mat = pixelStuff.getMat(curr.materialID);
			if (mat.flags & MaterialFlags::neverUpdate) { continue; }
			if (mat.flags & MaterialFlags::gravity) { pixelStuff.Gravity(curr, mat, c, r); }
		}
	}
}

