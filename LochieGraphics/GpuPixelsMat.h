#pragma once

#include <string>

#include "Colour.h"
#include "Serialisation.h"

namespace PixelsGPU
{
	class Material
	{
	public:

		inline static const std::string defaultSavePath = "Assets/Pixels/Materials/";
		inline static const std::string defaultExtension = ".pixmat";

		std::string name;
		std::string preUpdateFunction;
		std::string moveFunction;
		std::string filename;
		Colour defaultColour;
		int id;
		float density;
		bool affectedByGravity;
		bool empty; // Other pixels able to move to this one

		static std::string GetMovementCode(const std::vector<Material>& mats);
		static int GetLargestID(const std::vector<Material>& mats);
		static std::string GetMaterialInfoCode(const std::vector<Material>& mats);

		static std::string getFullPath(std::string filename);
		std::string getFullPath();

		Material() = default;
		Material(std::string _filename);
		void Load(std::string _filename);
		void Load(toml::table& table);
		void SaveAsFile();
		toml::table Serialise();
		void Reload();

		static void MaterialsGUI(std::vector<Material>& mats);
		void GUI();
	};
}
