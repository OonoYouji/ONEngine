#pragma once

/// engine
#include <cmath>


namespace ONEngine {

enum class GPUTimeStampID : uint32_t {
	VoxelTerrainRegularCell = 0,
	VoxelTerrainTransitionCell,
	VoxelTerrainEditorCompute,
	VoxelTerrainEditorBrushPreview,
	RenderingTotal,
	ShadowMap,
	MainScene,
	PostProcess,
	DebugDraw,
	PrefabDraw,
	MeshRendering,
	SpriteRendering,
	ParticleRendering,
	SkinMeshRendering,
	DissolveMeshRendering,
	Count
};

} /// namespace ONEngine