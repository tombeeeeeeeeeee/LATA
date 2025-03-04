#include "TestScene.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"

#include "Paths.h"

TestScene::TestScene()
{
}




void TestScene::Start()
{
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
	pixels[0][0].colour = { 1.0f, 1.0f, 1.0f, 1.0f };
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
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PixelData) * PIXELS_W * PIXELS_H, &pixels[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void TestScene::Update(float delta)
{

}

void TestScene::Draw(float delta)
{
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
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(PixelData) * PIXELS_W * PIXELS_H, &pixels[0]);

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
}

TestScene::~TestScene()
{
}
