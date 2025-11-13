#include "VoxelTerrainRenderingPipeline.h"

VoxelTerrainRenderingPipeline::VoxelTerrainRenderingPipeline(AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
}

VoxelTerrainRenderingPipeline::~VoxelTerrainRenderingPipeline() {}


void VoxelTerrainRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	
	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.ps.hlsl", L"ps_6_0", Shader::Type::ps);

	}

}

void VoxelTerrainRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

}
