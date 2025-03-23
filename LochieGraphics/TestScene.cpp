#include "TestScene.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"

#include "Utilities.h"

#include "Paths.h"

#include "EditorGUI.h"

TestScene::TestScene()
{
}




void TestScene::Start()
{
	// Would later load and save these to a file so that new materials could be made whenever and don't require a whole rebuild
	materialInfos = std::vector<MaterialInfo>{
		{ "air", { 197 / 255.0f, 222 / 255.0f, 227 / 255.0f, 1.0f }, 0.0f, MaterialFlags::neverUpdate},
		{ "sand", { 212 / 255.0f, 178 / 255.0f, 57 / 255.0f, 1.0f }, 1.0f, MaterialFlags::gravity},
	};


	for (size_t col = 0; col < PIXELS_W; col++)
	{
		for (size_t row = 0; row < PIXELS_H; row++)
		{
			PixelData& pixel = pixels[col][row];
			pixel.colour.x = (float)col / (PIXELS_W - 1);
			pixel.colour.y = (float)row / (PIXELS_H - 1);
			pixel.colour.z = 0.0f;
			//pixel.colour = { 0.1f, 0.1f, 0.1f, 1.0f };
		}
	}
	// Setting specific pixels colour for reference
	pixels[0][0].colour = { 1.0f, 1.0f, 1.0f, 1.0f }; // TODO: Do we want 0,0 do be considered left top or bottom, currently bottom left
	pixels[PIXELS_W - 1][0].colour = { 0.5f, 0.5f, 0.5f, 1.0f };
	pixels[PIXELS_W - 1][PIXELS_H - 1].colour = { 0.0f, 0.0f, 0.0f, 1.0f };


	texture = ResourceManager::CreateTexture(PIXELS_W, PIXELS_H, GL_SRGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);

	frameBuffer = new FrameBuffer(PIXELS_W, PIXELS_H, texture, nullptr, false);

	pixelShader = ResourceManager::LoadShader("simplePixel");
	pixelShader->Use();
	pixelShader->setInt("gridCols", PIXELS_W);
	pixelShader->setInt("gridRows", PIXELS_H);
	overlayShader = ResourceManager::LoadShader("Shaders/default.vert", "Shaders/simpleTexturedWithCutout.frag");

	// A quad
	quad.InitialiseQuad(1.0f);



	// Set up pixel colour SSBO, this is how the pixel colours are stored on the GPU
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// TODO: This shouldn't be a "PixelData" or something, as we wouldn't need to upload everything to the GPU (Probably just the colour)
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GpuPixelData) * PIXELS_W * PIXELS_H, &pixels[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

// TODO: Move this function
bool TestScene::SwapPixels(PixelData& a, PixelData& b) {

	// TODO: this maybe shouldn't be here? 
	if (a.updated || b.updated) {
		return false;
	}

	auto tempID = a.materialID;
	a.materialID = b.materialID;
	b.materialID = tempID;

	auto tempColour = a.colour;
	a.colour = b.colour;
	b.colour = tempColour;

	a.updated = getMat(a.materialID).flags != MaterialFlags::neverUpdate;
	b.updated = getMat(b.materialID).flags != MaterialFlags::neverUpdate;

	//auto tempUpdated = a.updated;
	//a.updated = b.updated;
	//b.updated = tempUpdated;

	return true;
}

MaterialInfo& TestScene::getNonConstMat(unsigned int index)
{
	// TODO: Should there be a debug / not found material info
	if (index > materialInfos.size()) {
		return materialInfos.front();
	}
	return materialInfos[index];
}

bool TestScene::getSpread()
{
	return spreadTest;
}

const MaterialInfo& TestScene::getMat(unsigned int index) const
{
	// TODO: Should there be a debug / not found material info
	if (index > materialInfos.size()) {
		return materialInfos.front();
	}
	return materialInfos[index];
}

bool TestScene::GravityDiagonalHelper(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r, bool spread) {
	auto nextC = c + ((spread) ? 1 : -1);
	if (nextC < 0 || nextC >= PIXELS_W) {
		return false;
	}
	auto& downNext = pixels[nextC][r - 1];
	const auto& downNextMat = getMat(downNext.materialID);
	if (downNextMat.density < mat.density) {
		if (SwapPixels(pixel, downNext)) {
			return true;
		}
	}
	return false;
}

void TestScene::SetCircleToMaterial(int x, int y, unsigned int materialID)
{
	for (size_t c = 0; c < PIXELS_W; c++)
	{
		for (size_t r = 0; r < PIXELS_H; r++) {
			if (sqrt(pow((float)c - x, 2) + pow((float)r - y, 2)) <= selectEditRadius) {
				pixels[c][r].materialID = materialID;
			}
		}
	}
}

void TestScene::UpdateSim()
{
	spreadTest = !spreadTest;
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
	for (size_t c = (spreadTest) ? 1 : PIXELS_W - 1; (spreadTest) ? c < PIXELS_W - 1 : c > 1; c += spreadTest ? 1 : -1)
	//for (size_t c = 1; c < PIXELS_W - 1; c++)
	{
		for (size_t r = 1; r < PIXELS_H - 1; r++)
		{
			auto& curr = pixels[c][r];
			if (curr.updated) { continue; }
			const auto& mat = getMat(curr.materialID);
			if (mat.flags & MaterialFlags::neverUpdate) { continue; }
			if (mat.flags & MaterialFlags::gravity) { Gravity(curr, mat, c, r); }
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

// TODO: Move function
void TestScene::Gravity(PixelData& pixel, const MaterialInfo& mat, unsigned int c, unsigned int r) {
	auto& down = pixels[c][r - 1];
	const auto& downMat = getMat(down.materialID);
	if (downMat.density < mat.density) {
		SwapPixels(pixel, down);
		return;
	}
	bool spread = true;
	if (GravityDiagonalHelper(pixel, mat, c, r, spread)) { return; }
	if (GravityDiagonalHelper(pixel, mat, c, r, !spread)) { return; }

}

void TestScene::Update(float delta)
{
	if (updateSim) {
		UpdateSim();
	}

	
	guiCursor = glm::ivec2{ cursorPos->x * PIXELS_W, cursorPos->y * PIXELS_H };
	if (!ImGui::GetIO().WantCaptureMouse && glfwGetMouseButton(renderSystem.window, GLFW_MOUSE_BUTTON_LEFT)) {
		SetCircleToMaterial(guiCursor.x, guiCursor.y, selectMat);
	}

	for (auto& c : pixels) {
		for (auto& i : c)
		{
			i.colour = getMat(i.materialID).defaultColour;
		}
	}
}

void TestScene::Draw(float delta)
{
	// Update the gpu version in preperation to send to GPU
	for (size_t c = 0; c < PIXELS_W; c++)
	{
		for (size_t r = 0; r < PIXELS_H; r++)
		{
			GpuPixels[c][r].colour = pixels[c][r].colour;
		}
	}
	


	// Note: Not using the render system for anything yet, haven't changed anything with it yet, should be fine either commented or not
	//renderSystem.Update(
	//	transforms,
	//	pointLights,
	//	spotlights,
	//	camera,
	//	delta
	//);

	// Update SSBO, the pixel colour data
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// TODO: This shouldn't be a "PixelData" or something, as we wouldn't need to upload everything to the GPU (Probably just the colour)
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GpuPixelData) * PIXELS_W * PIXELS_H, &GpuPixels[0]);

	// Note: The framebuffer is not required for the current set up, is here for the sake of it at the moment
	// Bind framebuffer for drawing pixels
	frameBuffer->Bind();
	glViewport(0, 0, PIXELS_W, PIXELS_H);
	
	// Draw pixels
	pixelShader->Use();
	quad.Draw();

	// Unbind framebuffer
	FrameBuffer::Unbind();
	// TODO: Is there a rendersystem function for this, if not maybe there should be something similar
	glViewport(0, 0, renderSystem.SCREEN_WIDTH / renderSystem.superSampling, renderSystem.SCREEN_HEIGHT / renderSystem.superSampling);


	// Draw framebuffer texture to screen
	overlayShader->Use();
	overlayShader->setFloat("material.alpha", 1.0f);

	texture->Bind(1);
	overlayShader->setSampler("material.albedo", 1);

	quad.Draw();

}

void TestScene::GUI()
{
	if (!ImGui::Begin("Test Scene!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
	}

	ImGui::Checkbox("Update Sim", &updateSim);
	ImGui::SameLine();
	if (ImGui::Button("Update Frame")) {
		UpdateSim();
	}

	ImGui::BeginDisabled();
	
	int amountOfAir = 0;
	for (const auto& c : pixels)
	{
		for (const auto& i : c)
		{
			if (i.materialID == 0) { amountOfAir++; }
		}
	}
	ImGui::InputInt("Amount of Air", &amountOfAir);

	ImGui::EndDisabled();

	ImGui::ColorEdit3("Colour to set", &pickerColour.x);

	ImGui::DragFloat("Select Edit Radius", &selectEditRadius);

	ImGui::InputScalar("Material", ImGuiDataType_U32, &selectMat);

	glm::ivec2 cursorPixelPos = { Utilities::PositiveMod(guiCursor.x, PIXELS_W), Utilities::PositiveMod(guiCursor.y, PIXELS_H) };


	if (ImGui::Button("Set everything to above material")) {
		for (auto& r : pixels) {
			for (auto& i : r)
			{
				i.materialID = selectMat;
			}
		}
	}


	if (ImGui::Button("Set select to above material")) {
		SetCircleToMaterial(guiCursor.x, guiCursor.y, selectMat);
	}





	if (ImGui::Button("Set everything to colour")) {
		for (auto& r : pixels) {
			for (auto& i : r)
			{
				i.colour = glm::vec4(pickerColour, 1.0f);
			}
		}
	}

	if (ImGui::Button("Set everything to default material colours")) {
		for (auto& r : pixels) {
			for (auto& i : r)
			{
				i.colour = getMat(i.materialID).defaultColour;
			}
		}
	}

	ImGui::DragInt2("Gui Cursor", &guiCursor.x);

	if (ImGui::CollapsingHeader("Selected Info")) {
		ImGui::SeparatorText("Pixel Info");
		auto& currPixel = pixels[cursorPixelPos.x][cursorPixelPos.y];

		currPixel.GUI();

		ImGui::SeparatorText("Pixel Material Info");
		getNonConstMat(currPixel.materialID).GUI();
	}

	ImGui::End();
}

TestScene::~TestScene()
{
}

MaterialInfo::MaterialInfo(std::string _name, glm::vec4(_colour), float _density, unsigned int _flags) :
	name(_name),
	defaultColour(_colour),
	density(_density),
	flags(_flags)
{
}

void MaterialInfo::GUI()
{
	std::string tag = Utilities::PointerToString(this);

	ImGui::InputText(("Name##" + tag).c_str(), &name);
	ImGui::ColorEdit4(("Default Colour##" + tag).c_str(), &defaultColour.x);
	ImGui::CheckboxFlags(("Never Update##" + tag).c_str(), &flags, MaterialFlags::neverUpdate);
	ImGui::CheckboxFlags(("Gravity##" + tag).c_str(), &flags, MaterialFlags::gravity);
	ImGui::InputFloat(("Density##" + tag).c_str(), &density);
}

void PixelData::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::ColorEdit4(("Colour##" + tag).c_str(), &colour.x);
	// TODO: Should be some better GUI option for this
	ImGui::InputScalar("Material ID", ImGuiDataType_U32, &materialID);
}
