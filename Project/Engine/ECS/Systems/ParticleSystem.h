#pragma once

#include <vector>
#include <memory>
#include <d3d12.h>
#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"

namespace Engine::Graphics { struct RenderContext; }

namespace Engine::ECS {

///
/// GPU駆動パーティクルシステム
///
class ParticleSystem final : public System {
public:
	struct EmitterState {
		std::unique_ptr<Graphics::StructuredBuffer> particleBuffer;
		std::unique_ptr<Graphics::ConstantBuffer> updateCB; // エミッターごとに定数バッファを持つ
		std::vector<std::unique_ptr<Graphics::StructuredBuffer>> stagingBuffers;
		uint32_t count = 0;
		bool initialized = false;
	};

	void Update(Registry& registry) override;
	void Render(Registry& registry, const Graphics::RenderContext& context);
	void Initialize(Graphics::RenderDevice* device);
	void Shutdown();

	void UpdateMeshInfoBuffer(); // publicに移動
	D3D12_GPU_VIRTUAL_ADDRESS GetMeshInfoBufferAddress() const { return meshInfoBuffer_->GetResource()->GetGPUVirtualAddress(); }

private:

private:
	Graphics::RenderDevice* device_ = nullptr;
	std::unordered_map<Entity, EmitterState> emitters_;

	std::unique_ptr<Graphics::StructuredBuffer> meshInfoBuffer_;

	// Compute Shader 用
	struct UpdateParams {
		float dt;
		Engine::Math::Vector3 emitterPos;
		uint32_t totalParticles;

		float seed;
		float speed;
		float speedRandom;
		float lifetime;

		float lifetimeRandom;
		float spreadAngle;
		float gravity;
		float startScale;

		float endScale;
		uint32_t modelIndex;
		uint32_t textureIndex;
		float padding;

		Engine::Math::Vector4 startColor;
		Engine::Math::Vector4 endColor;
	};
	float time_ = 0.0f; // シード用
};

} // namespace Engine::ECS
