#include "GpuPixelsSim.h"

#include "SceneManager.h"

#include "Graphics.h"
#include "Paths.h"
#include "EditorGUI.h"
#include "Colour.h"
#include "Utilities.h"

#include <functional>
#include <filesystem>

void PixelsGPU::Simulation::LoadComputeShaders()
{
	// Need to make sure these are all good limit wise when running on a worse device than mine
	int workGroupSize[3] = { 0 };
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

	int workGroupCounts[3] = { 0 };
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);

	int workGroupInvocations;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInvocations);

	int maxComputeSSBOs;
	glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &maxComputeSSBOs);

	if (maxComputeSSBOs < 10)
	{
		// We gonnna have an issue
		__debugbreak();
	}

	if (updatePixels)
	{
		updatePixels->DeleteProgram();
	}
	const std::string updateFilepath = Paths::importShaderLocation + "update" + Paths::computeExtension;
	std::string updateCode = Utilities::FileToString(updateFilepath);
	updateCode = Shader::PreProcessShaderCode(updateCode, updateFilepath);
	
	std::string movementCode = Material::GetMovementCode(materials);
	const std::string movementIdentifier = "// CODE INSERT: MOVEMENT";
	size_t movementSlot = updateCode.find(movementIdentifier);
	updateCode.replace(movementSlot, movementIdentifier.size(), movementCode);
	
	std::string materialInfoCode = Material::GetMaterialInfoCode(materials);
	const std::string materialInfoIdentifier = "// CODE INSERT: MATERIAL_INFO";
	size_t materialInfoSlot = updateCode.find(materialInfoIdentifier);
	updateCode.replace(materialInfoSlot, materialInfoIdentifier.size(), materialInfoCode);

	updatePixels = ComputeShader::CreateCustomComputeShader(updateCode);
	if (preUpdate)
	{
		preUpdate->DeleteProgram();
	}
	preUpdate = new ComputeShader(Paths::importShaderLocation + "preUpdate" + Paths::computeExtension);
	if (placeCircle)
	{
		placeCircle->DeleteProgram();
	}
	placeCircle = new ComputeShader(Paths::importShaderLocation + "drawCircle" + Paths::computeExtension);
	if (testCompute)
	{
		testCompute->DeleteProgram();
	}
	testCompute = new ComputeShader(Paths::importShaderLocation + "testPixelCompute" + Paths::computeExtension);
	if (testCompute2)
	{
		testCompute2->DeleteProgram();
	}
	testCompute2 = new ComputeShader(Paths::importShaderLocation + "testPixelCompute2" + Paths::computeExtension);
}

void PixelsGPU::Simulation::InitialisePlayer()
{
	glGenBuffers(1, &playerInfoSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, playerInfoSSBO);

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PixelsGPU::PlayerInfo), nullptr, GL_STATIC_COPY);
	glClearNamedBufferData(playerInfoSSBO, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, playerInfoSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void PixelsGPU::Simulation::InitialiseMaterials()
{
	materials.clear();
	for (auto& i : std::filesystem::directory_iterator(Material::defaultSavePath))
	{
		if (i.path().filename().string() == ".FolderNeedsToExist")
		{
			continue;
		}
		materials.emplace_back(i.path().stem().string());
	}
	for (auto& mat : materials)
	{
		mat.SaveAsFile();
	}
}

void PixelsGPU::Simulation::InitialiseChunks()
{
	//constexpr int r = 1;
	//for (int x = -r; x < r + 1; x++)
	//{
	//	for (int y = -r; y < r + 1; y++)
	//	{
	//		chunks.emplace_back(glm::ivec2(x, y));
	//	}
	//}
	//constexpr int r = 4;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			CreateChunk(glm::ivec2(x, y));
		}
	}
	PixelsGPU::CellPixel cell;
	cell.matID = 1;
	cell.colour = 0xFFFFFFFF;
	SetCircleTo(0, 0, 999999999.0f, cell);
}

void PixelsGPU::Simulation::Update(float delta)
{
	timer += delta;
	preUpdate->Use();
	preUpdate->setInt("gridCols", chunkWidth);
	preUpdate->setInt("gridRows", chunkHeight);
	preUpdate->setInt("updateCount", updateCount);

	updatePixels->Use();
	updatePixels->setInt("gridCols", chunkWidth);
	updatePixels->setInt("gridRows", chunkHeight);
	updatePixels->setInt("updateCount", updateCount);

	testCompute->Use();
	bool left = glfwGetKey(SceneManager::window, GLFW_KEY_A) == GLFW_PRESS;
	bool right = glfwGetKey(SceneManager::window, GLFW_KEY_D) == GLFW_PRESS;
	bool up = glfwGetKey(SceneManager::window, GLFW_KEY_W) == GLFW_PRESS;
	bool down = glfwGetKey(SceneManager::window, GLFW_KEY_S) == GLFW_PRESS;
	testCompute->setVec2("currentInput", glm::vec2((left ? -1 : 0) + (right ? 1 : 0), (up ? 1 : 0) + (down ? -1 : 0)));

	for (int i = 0; i < subUpdates; i++)
	{
		preUpdate->Use();
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		for (auto& chunk : chunks)
		{
			chunk.BindSSBO(chunk.centreIndexSSBO);
			preUpdate->setBool("readSsboFirst", chunk.readSsboFirst);
			preUpdate->setIVec2("chunkCoords", chunk.coords);
			preUpdate->Run(chunkWidth / computeLocalSizeX, chunkHeight / computeLocalSizeY, computeLocalSizeZ, 0);
			chunk.SwitchReadWriteSSBOs();
		}

		if (debugTest)
		{
			testCompute->Run(1, 1, 1, 0);
		}

		updatePixels->Use();
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		for (auto& chunk : chunks)
		{
			chunk.BindSSBO(Chunk::centreIndexSSBO);

			int nearChunkStatus[std::size(Chunk::orderedNearIndexSSBO)];
			for (int nearI = 0; nearI < std::size(Chunk::orderedNearIndexSSBO); ++nearI)
			{
				const Chunk* nearChunk = getChunkAt(chunk.coords + Chunk::orderedNearChunksLocalOffsets[nearI]);
				if (nearChunk)
				{
					nearChunk->BindSSBO(Chunk::orderedNearIndexSSBO[nearI]);
				}
				nearChunkStatus[nearI] = (nearChunk) ? (nearChunk->readSsboFirst ? 1 : 2) : 0;
			}
			updatePixels->setIntArray("nearChunkStatus", nearChunkStatus, std::size(Chunk::orderedNearIndexSSBO));

			updatePixels->setBool("readSsboFirst", chunk.readSsboFirst);
			updatePixels->setIVec2("chunkCoords", chunk.coords);
			updatePixels->Run(chunkWidth / computeLocalSizeX, chunkHeight / computeLocalSizeY, computeLocalSizeZ, 0);
		}
		for (auto& chunk : chunks)
		{
			chunk.SwitchReadWriteSSBOs();
		}
		++updateCount;
	}
}

const PixelsGPU::Chunk* PixelsGPU::Simulation::getChunkAt(glm::ivec2 chunkCoords)
{
	for (const auto& chunk : chunks)
	{
		if (chunk.coords == chunkCoords)
		{
			return &chunk;
		}
	}
	return nullptr;
}

PixelsGPU::Chunk& PixelsGPU::Simulation::CreateChunk(glm::ivec2 chunkCoords)
{
	auto& chunk = chunks.emplace_back(chunkCoords);
	chunk.Initialise();
	return chunk;
}

void PixelsGPU::Simulation::DestroyChunk(glm::ivec2 chunkCoords)
{
	for (auto i = chunks.begin(); i != chunks.end(); ++i)
	{
		if (i->coords == chunkCoords)
		{
			i->Deinitalise();
			chunks.erase(i);
			return;
		}
	}
}

size_t PixelsGPU::Chunk::CalculateSsboSize() const
{
	return sizeof(PixelsGPU::CellPixel) * chunkWidth * chunkHeight * 2;
}

void PixelsGPU::Simulation::SetCircleTo(glm::ivec2 pos, float radius, PixelsGPU::CellPixel cell)
{
	placeCircle->Use();
	placeCircle->setInt("gridCols", chunkWidth);
	placeCircle->setInt("gridRows", chunkHeight);

	placeCircle->setIVec2("centerCoords", pos);
	placeCircle->setFloat("radius", radius);
	placeCircle->setInt("pixel.matID", cell.matID);
	placeCircle->setVec2("pixel.vel", cell.vel);
	placeCircle->setBool("velocityAdd", true);
	if (cell.matID == 2)
	{
		placeCircle->setUnsignedInt("pixel.colour", Colour::RGBAU32FromRGBA01(glm::vec4(sin(timer * PI) / 4.0f + 0.75f, 0.76f, sin(timer * PI * 4) / 6.0f + 0.17f, 1.0)));
	}
	else if (cell.matID != 0)
	{
		placeCircle->setUnsignedInt("pixel.colour", Colour::RGBAU32FromRGBA01(glm::vec4(0.9, 0.9, 0.9, 1.0f)));
	}
	else
	{
		placeCircle->setUnsignedInt("pixel.colour", Colour::RGBAU32FromRGBA01(glm::vec4(0, 0, 0, 1.0f)));
	}

	placeCircle->setFloat("timer", timer);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	for (auto& chunk : chunks)
	{
		chunk.BindSSBO(Chunk::centreIndexSSBO);
		placeCircle->setBool("readSsboFirst", chunk.readSsboFirst);
		placeCircle->setIVec2("chunkCoords", chunk.coords);
		placeCircle->Run(chunkWidth / computeLocalSizeX, chunkHeight / computeLocalSizeY, computeLocalSizeZ, 0);
		chunk.SwitchReadWriteSSBOs();
	}
}

void PixelsGPU::Simulation::SetCircleTo(int x, int y, float radius, PixelsGPU::CellPixel cell)
{
	SetCircleTo(glm::ivec2(x, y), radius, cell);
}

void PixelsGPU::Simulation::GUI()
{
	ImGui::Checkbox("debug test", &debugTest);
	ImGui::InputInt("Sub updates", &subUpdates);
	if (ImGui::Button("Re load materials"))
	{
		InitialiseMaterials();
	}
	ImGui::SameLine();
	if (ImGui::Button("Re load compute shaders"))
	{
		LoadComputeShaders();
	}
	if (ImGui::Button("Re load material and compute shaders"))
	{
		InitialiseMaterials();
		LoadComputeShaders();
	}
	PixelsGPU::Material::MaterialsGUI(materials);
}
