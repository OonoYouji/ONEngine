#pragma once

/// engine
#include <cmath>


namespace ONEngine {

enum class GPUTimeStampID : uint32_t {
	VoxelTerrainRegularCell = 0,
	VoxelTerrainTransitionCell,
	VoxelTerrainEditorCompute,
	Count
};

} /// namespace ONEngine