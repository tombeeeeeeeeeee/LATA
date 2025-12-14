#include "RaytracerThingScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"


#include "imgui.h"

#include <iostream>

constexpr int imageWidth = 1024;
constexpr int imageHeight = 1024;

void RaytracerThingScene::DoRayTraceSetup()
{
	tracerShader = ResourceManager::LoadShader("locTracer");
	simpleShader = ResourceManager::LoadShader("ui");

}

void RaytracerThingScene::Start()
{
	quad.InitialiseQuad(1.0f);
	//texture = ResourceManager::LoadTexture("images/T_MissingTexture.png", Texture::Type::albedo);
	//simpleShader = ResourceManager::LoadShader("ui");
	texture = ResourceManager::CreateTexture(imageWidth, imageHeight, GL_RGBA, nullptr, GL_CLAMP_TO_BORDER, GL_UNSIGNED_BYTE, false, GL_NEAREST, GL_NEAREST);

	frameBuffer = new FrameBuffer(imageWidth, imageHeight, texture, nullptr, false);
	DoRayTraceSetup();
	displayGUI = false;
}

static bool firstUpdate = false;

void RaytracerThingScene::Update(float delta)
{
	if (!firstUpdate)
	{
		
		// This should really jsut be avaiable in start
		SceneManager::SwitchToWindowMode(WindowModes::windowed);
		*windowWidth = imageWidth;
		*windowHeight = imageHeight;
		SceneManager::SwitchToWindowMode(WindowModes::windowed);
		
		firstUpdate = true;

	}

	bool prevRedrawKeyDown = redrawKeyDown;
	redrawKeyDown = glfwGetKey(SceneManager::window, GLFW_KEY_R) == GLFW_PRESS;
	if (redrawKeyDown && !prevRedrawKeyDown)
	{
		redraw = true;
	}

	timer += delta;
}

void RaytracerThingScene::Draw(float delta)
{
	//renderSystem.Update(transforms, pointLights, spotlights, camera, delta, nullptr, nullptr, nullptr, quad, nullptr, nullptr);
	if (redraw)
	{
		DoRayTraceSetup();
		redraw = false;
		std::cout << '\7';
	}

	frameBuffer->Bind();
	glViewport(0, 0, imageWidth, imageHeight);

	tracerShader->Use();
	//glm::mat4 model = glm::mat4(1.0f);
	//tracerShader->setMat4("model", model);
	//tracerShader->setMat4("vp", SceneManager::viewProjection);
	float aspectRatio = (float)imageWidth / (float)imageHeight;

	auto focal_length = 1.0;
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (double(imageWidth) / imageHeight);
	auto camera_center = glm::vec3(0, 0, 0);

	// Calculate the vectors across the horizontal and down the vertical viewport edges.
	auto viewport_u = glm::vec3(viewport_width, 0, 0);
	auto viewport_v = glm::vec3(0, -viewport_height, 0);

	// Calculate the horizontal and vertical delta vectors from pixel to pixel.
	auto pixel_delta_u = viewport_u / (float)imageWidth;
	auto pixel_delta_v = viewport_v / (float)imageHeight;

	// Calculate the location of the upper left pixel.
	auto viewport_upper_left = camera_center
		- glm::vec3(0, 0, focal_length) - viewport_u / 2.0f - viewport_v / 2.0f;
	auto pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

	tracerShader->setInt("width", imageWidth);
	tracerShader->setInt("height", imageHeight);

	tracerShader->setVec3("pixel00_loc", pixel00_loc);
	tracerShader->setVec3("pixel_delta_u", pixel_delta_u);
	tracerShader->setVec3("pixel_delta_v", pixel_delta_v);
	tracerShader->setVec3("camera_center", camera_center);
	tracerShader->setFloat("timer", timer);
	

	//tracerShader->setFloat("aspectRatio", aspectRatio);
	quad.Draw();

	frameBuffer->Unbind();
	glViewport(0, 0, *windowWidth, *windowHeight);


	texture->Bind(1);
	simpleShader->Use();
	simpleShader->setSampler("material.albedo", 1);
	quad.Draw();

}

void RaytracerThingScene::GUI()
{
	ImGui::SliderFloat("timer", &timer, 0.0f, 4.0f);
}
