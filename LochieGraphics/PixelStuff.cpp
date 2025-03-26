#include "PixelStuff.h"

#include "EditorGUI.h"

#include "Utilities.h"


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

	// neverUpdate pixels should not be marked as updated, they can be 'moved' around multiple times
	// Other types of pixels should be marked as updated if they are moved
	a.updated = getMat(a.materialID).flags != MaterialFlags::neverUpdate;
	b.updated = getMat(b.materialID).flags != MaterialFlags::neverUpdate;

	return true;
}

PixelStuff::MaterialInfo& PixelStuff::getNonConstMat(unsigned int index)
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

const PixelStuff::MaterialInfo& PixelStuff::getMat(unsigned int index) const
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

void PixelStuff::SetCircleToMaterial(int x, int y, float radius, unsigned int materialID)
{
	for (size_t c = 0; c < PIXELS_W; c++)
	{
		for (size_t r = 0; r < PIXELS_H; r++) {
			if (sqrt(pow((float)c - x, 2) + pow((float)r - y, 2)) <= radius) {
				chunk.pixels[c][r].materialID = materialID;
			}
		}
	}
}

unsigned int PixelStuff::AmountOf(unsigned int materialID) const
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
			GpuPixels[c][r].colour = glm::vec4(chunk.pixels[c][r].colour, 1.0f);
		}
	}
}

void PixelStuff::SetSimpleMaterials()
{
	// Would later load and save these to a file so that new materials could be made whenever and don't require a whole rebuild
	materialInfos = std::vector<MaterialInfo>{
		{ "air", { 197 / 255.0f, 222 / 255.0f, 227 / 255.0f, 1.0f }, 0.0f, MaterialFlags::neverUpdate},
		{ "sand", { 212 / 255.0f, 178 / 255.0f, 57 / 255.0f, 1.0f }, 1.0f, MaterialFlags::gravity},
	};
}

void PixelStuff::SetDebugColours()
{
	for (size_t col = 0; col < PIXELS_W; col++)
	{
		for (size_t row = 0; row < PIXELS_H; row++)
		{
			PixelData& pixel = chunk.pixels[col][row];
			pixel.colour.x = (float)col / (PIXELS_W - 1);
			pixel.colour.y = (float)row / (PIXELS_H - 1);
			pixel.colour.z = 0.0f;
			//pixel.colour = { 0.1f, 0.1f, 0.1f, 1.0f };
		}
	}
	// Setting specific pixels colour for reference
	chunk.pixels[0][0].colour = { 1.0f, 1.0f, 1.0f }; // TODO: Do we want 0,0 do be considered left top or bottom, currently bottom left
	chunk.pixels[PIXELS_W - 1][0].colour = { 0.5f, 0.5f, 0.5f };
	chunk.pixels[PIXELS_W - 1][PIXELS_H - 1].colour = { 0.0f, 0.0f, 0.0f };

}

void PixelStuff::SetEverythingTo(unsigned int materialID)
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
			i.colour = glm::vec4(colour, 1.0f);
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

void PixelStuff::PixelGUI(int x, int y)
{
	auto& currPixel = chunk.pixels[x][y];

	currPixel.GUI();

	ImGui::SeparatorText("Pixel Material Info");
	getNonConstMat(currPixel.materialID).GUI();
}

// TODO: Move function
void PixelStuff::Gravity(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r) {
	auto& down = chunk.pixels[c][r - 1];
	const auto& downMat = getMat(down.materialID);
	if (downMat.density < mat.density) {
		SwapPixels(pixel, down);
		return;
	}
	bool spread = true;
	if (GravityDiagonalHelper(pixel, mat, c, r, spread)) { return; }
	if (GravityDiagonalHelper(pixel, mat, c, r, !spread)) { return; }

}

PixelStuff::MaterialInfo::MaterialInfo(std::string _name, glm::vec4(_colour), float _density, unsigned int _flags) :
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
	ImGui::ColorEdit4(("Default Colour##" + tag).c_str(), &defaultColour.x);
	ImGui::CheckboxFlags(("Never Update##" + tag).c_str(), &flags, MaterialFlags::neverUpdate);
	ImGui::CheckboxFlags(("Gravity##" + tag).c_str(), &flags, MaterialFlags::gravity);
	ImGui::InputFloat(("Density##" + tag).c_str(), &density);
}

void PixelStuff::PixelData::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::ColorEdit4(("Colour##" + tag).c_str(), &colour.x);
	// TODO: Should be some better GUI option for this
	ImGui::InputScalar("Material ID", ImGuiDataType_U32, &materialID);
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
	for (unsigned int c = (pixelStuff.spreadTest) ? 1 : PIXELS_W - 1; (pixelStuff.spreadTest) ? c < PIXELS_W - 1 : c > 1; c += pixelStuff.spreadTest ? 1 : -1)
		//for (size_t c = 1; c < PIXELS_W - 1; c++)
	{
		for (unsigned int r = 1; r < PIXELS_H - 1; r++)
		{
			auto& curr = pixels[c][r];
			if (curr.updated) { continue; }
			const auto& mat = pixelStuff.getMat(curr.materialID);
			if (mat.flags & MaterialFlags::neverUpdate) { continue; }
			if (mat.flags & MaterialFlags::gravity) { pixelStuff.Gravity(curr, mat, c, r); }
		}
	}
	// Extra gravity (falling non directly down, like diagonally)
	//for (size_t c = 1; c < PIXELS_W - 1; c++)
	//{
	//	for (size_t r = 1; r < PIXELS_H - 1; r++)
	//	{
	//		//continue;
	//		auto& curr = pixels[c][r];
	//		if (curr.updated) { continue; }
	//		const auto& mat = getMat(curr.materialID);
	//		if (mat.flags & MaterialFlags::neverUpdate) { continue; }
	//		if (mat.flags & MaterialFlags::gravity) {
	//			bool spread = getSpread();
	//			if (GravityDiagonalHelper(curr, mat, c, r, spread)) { continue; }
	//			//if (GravityDiagonalHelper(curr, mat, c, r, !spread)) { continue; }

	//		}
	//	}
	//}
}

