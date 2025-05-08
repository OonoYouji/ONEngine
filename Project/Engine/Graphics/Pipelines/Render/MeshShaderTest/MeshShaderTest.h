#pragma once
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"

class MeshShaderTest : public IRenderingPipeline {
public:

	MeshShaderTest() = default;
	~MeshShaderTest() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(DxCommand* _dxCommand, class EntityComponentSystem* _pEntityComponentSystem, class Camera* _camera) override;

private:

	struct VSInput {
		Vector4 position;
		Vector2 uv;
		Vector3 normal;
	};

	struct Index {
		uint32_t indices[3];
	};

	struct BufferLength {
		uint32_t vertexInputLength;
		uint32_t indexLength;
	};

	StructuredBuffer<VSInput> vertexBuffer_;
	StructuredBuffer<Index> indexBuffer_;
	ConstantBuffer<BufferLength> bufferLength_;
};

