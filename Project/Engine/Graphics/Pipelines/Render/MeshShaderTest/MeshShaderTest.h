#pragma once
#include "../../Interface/IRenderingPipeline.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/ByteAddressBuffer.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"

#include "Game/Objects/Terrain/Terrain.h"


class MeshShaderTest : public IRenderingPipeline {
public:

	MeshShaderTest() = default;
	~MeshShaderTest() = default;

	void Initialize(ShaderCompiler* _shaderCompiler, class DxManager* _dxManager) override;
	void Draw(const std::vector<IEntity*>& _entities, class Camera* _camera, DxCommand* _dxCommand) override;

private:

	uint32_t PackPrimitive(uint32_t i0, uint32_t i1, uint32_t i2);

private:

	struct VSInput {
		Vector4 position;
		Vector3 normal;
		Vector2 uv;
	};

	struct Index {
		uint32_t index;
	};

	struct BufferLength {
		uint32_t vertexInputLength;
		uint32_t indexLength;
	};

	struct meshInfo {
		uint32_t meshletOffset;
	};

	StructuredBuffer<VSInput> vertexBuffer_;
	StructuredBuffer<Index> primitiveBuffer_;
	StructuredBuffer<meshopt_Meshlet> meshletBuffer_;
	ConstantBuffer<meshInfo> meshInfoBuffer_;
	StructuredBuffer<uint32_t> uniqueVertexIndices_;
};

