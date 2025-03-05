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
static void SwapPixels(PixelData& a, PixelData& b) {
	auto tempID = a.materialID;
	a.materialID = b.materialID;
	b.materialID = tempID;

	auto tempColour = a.colour;
	a.colour = b.colour;
	b.colour = tempColour;
}

void TestScene::Update(float delta)
{
	if (!UpdateSim) {
		return;
	}
	even = !even;
	// For now, just don't update border pixels
	for (size_t c = 1; c < PIXELS_W - 1; c++)
	{
		for (size_t r = 1; r < PIXELS_H - 1; r++)
		{
			auto& curr = pixels[c][r];
			const auto& mat = materialInfos[curr.materialID];

			if (mat.flags & MaterialFlags::neverUpdate) { continue; }

			if (mat.flags & MaterialFlags::gravity) {
				auto& down = pixels[c][r - 1];
				const auto& downMat = materialInfos[down.materialID];
				if (downMat.density < mat.density) {
					SwapPixels(curr, down);
				}
				else {
					auto& downNext = pixels[c + ((even) ? 1 : -1)][r - 1];
					const auto& downNextMat = materialInfos[downNext.materialID];
					// TODO: Do we want to check the space above it as well to make sure it can "travel" okay
					if (downNextMat.density < mat.density) {
						SwapPixels(curr, downNext);
					}
					else {
						auto& downOtherNext = pixels[c - ((even) ? 1 : -1)][r - 1];
						const auto& downOtherNextMat = materialInfos[downOtherNext.materialID];
						if (downOtherNextMat.density < mat.density) {
							SwapPixels(curr, downOtherNext);
						}
					}
				}
				// Could check other valid spots to move to here
			}
		}
	}

	if (glfwGetKey(renderSystem.window, GLFW_KEY_UP) == GLFW_PRESS) { guiCursor.y += 1; }
	if (glfwGetKey(renderSystem.window, GLFW_KEY_DOWN) == GLFW_PRESS) { guiCursor.y -= 1; }
	if (glfwGetKey(renderSystem.window, GLFW_KEY_RIGHT) == GLFW_PRESS) { guiCursor.x += 1; }
	if (glfwGetKey(renderSystem.window, GLFW_KEY_LEFT) == GLFW_PRESS) { guiCursor.x -= 1; }

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

	ImGui::Checkbox("Update Sim", &UpdateSim);

	ImGui::ColorEdit3("Colour to set", &pickerColour.x);

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
				i.colour = materialInfos[i.materialID].defaultColour;
			}
		}
	}

	ImGui::DragInt2("Gui Cursor", &guiCursor.x);


	ImGui::SeparatorText("Pixel Info");
	glm::ivec2 temp = { Utilities::PositiveMod(guiCursor.x, PIXELS_W), Utilities::PositiveMod(guiCursor.y, PIXELS_H) };
	auto& currPixel = pixels[temp.x][temp.y];

	currPixel.colour.y -= 0.1f;

	currPixel.GUI();

	ImGui::SeparatorText("Pixel Material Info");
	auto& currMat = materialInfos[currPixel.materialID];
	currMat.GUI();

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
	ImGui::CheckboxFlags(("Never Update##" + tag).c_str(), &flags, MaterialFlags::gravity);
	ImGui::InputFloat(("Density##" + tag).c_str(), &density);
}

void PixelData::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::ColorEdit4(("Colour##" + tag).c_str(), &colour.x);
	// TODO: Should be some better GUI option for this
	ImGui::InputScalar("Material ID", ImGuiDataType_U32, &materialID);
}
