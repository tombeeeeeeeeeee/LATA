#include "GpuPixelSimScene.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"
#include "SceneManager.h"
#include "EditorGUI.h"

#include "Paths.h"

void GpuPixelSimScene::Start()
{
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(CellPixel) * width * height, nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	texture = ResourceManager::CreateTexture(width, height, GL_RGBA, nullptr, GL_CLAMP_TO_BORDER, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);

	frameBuffer = new FrameBuffer(width, height, texture, nullptr, false);

	//displayGUI = false;

	pixelShader = ResourceManager::LoadShader("simplePixelGpu");
	simple2dShader = ResourceManager::LoadShader("ui");
	temp1 = new ComputeShader(Paths::importShaderLocation + "testPixelCompute" + Paths::computeExtension);

	quad.InitialiseQuad(1.0f);
}

void GpuPixelSimScene::Update(float delta)
{
}

static bool doThing = false;

void GpuPixelSimScene::Draw(float delta)
{
	if (doThing)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

		doThing = false;
		temp1->Run(width * height, 1u, 1u);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	frameBuffer->Bind();
	glViewport(0, 0, width, height);

	pixelShader->Use();
	pixelShader->setInt("gridCols", width);
	pixelShader->setInt("gridRows", height);
	quad.Draw();

	frameBuffer->Unbind();
	glViewport(0, 0, *windowWidth, *windowHeight);

	simple2dShader->Use();
	texture->Bind(1);
	simple2dShader->setSampler("material.albedo", 1);
	quad.Draw();
}

void GpuPixelSimScene::GUI()
{
	if (ImGui::Button("Test"))
	{
		doThing = true;
	}
}
