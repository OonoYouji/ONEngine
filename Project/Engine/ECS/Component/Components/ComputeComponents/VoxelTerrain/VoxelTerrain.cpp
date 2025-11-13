#include "VoxelTerrain.h"

/// engine
#include "Engine/Core/Utility/Utility.h"

VoxelTerrain::VoxelTerrain() {}
VoxelTerrain::~VoxelTerrain() {}

void COMP_DEBUG::PrintVoxelTerrainInfo(const VoxelTerrain& _voxelTerrain) {
	if(!_voxelTerrain.enable) {
		Console::Log("VoxelTerrain is disabled.");
		return;
	}

}

void from_json(const nlohmann::json& _j, VoxelTerrain& _voxelTerrain) {
	_voxelTerrain.enable = _j.value("enable", 1);
}

void to_json(nlohmann::json& _j, const VoxelTerrain& _voxelTerrain) {
	/// VoxelTerrain -> Json
	_j = {
		{ "type", "VoxelTerrain" },
		{ "enable", _voxelTerrain.enable },
	};
}