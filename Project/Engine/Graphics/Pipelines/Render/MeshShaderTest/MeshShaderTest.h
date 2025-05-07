#pragma once
#include "../../Interface/IRenderingPipeline.h"

class MeshShaderTest : public IRenderingPipeline {
public:

	MeshShaderTest() = default;
	~MeshShaderTest() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(DxCommand* _dxCommand, class EntityComponentSystem* _pEntityComponentSystem, class Camera* _camera) override;

private:
};

