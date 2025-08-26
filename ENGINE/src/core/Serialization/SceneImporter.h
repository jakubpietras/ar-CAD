#pragma once
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>

using json = nlohmann::ordered_json;

namespace ar
{
	class SceneImporter
	{
	public:
		static bool Import(const std::string& filepath);
	private:
		static std::unordered_map<uint32_t, entt::entity> m_PointMap;
	};
}

