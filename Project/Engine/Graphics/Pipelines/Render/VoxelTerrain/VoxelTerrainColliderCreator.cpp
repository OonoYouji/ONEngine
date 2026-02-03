#include "VoxelTerrainColliderCreator.h"
#include "VoxelTerrainColliderCreator.h"

namespace ONEngine {

VoxelTerrainColliderCreator::VoxelTerrainColliderCreator() {

}

void VoxelTerrainColliderCreator::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		
	}

}

void VoxelTerrainColliderCreator::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {
}

}
