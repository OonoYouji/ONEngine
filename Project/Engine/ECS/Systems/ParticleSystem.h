#pragma once

#include <vector>
#include <memory>
#include <d3d12.h>
#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Engine/Graphics/Resource/GpuBuffer.h"
#include "Engine/Graphics/Resource/ConstantBuffer.h"
#include "Schema/Schema.h"

namespace Engine::Graphics { struct RenderContext; }

namespace Engine::ECS {

///
/// GPU駆動パーティクルシステム
///
class ParticleSystem final : public System {
public:
	struct EmitterState {
		std::unique_ptr<Graphics::StructuredBuffer> particleBuffer;
		std::vector<std::unique_ptr<Graphics::StructuredBuffer>> stagingBuffers;
		uint32_t count = 0;
		bool initialized = false;
	};

	void Update(Registry& registry) override;
	void Render(Registry& registry, const Graphics::RenderContext& context);
	void Initialize(Graphics::RenderDevice* device);
	void Shutdown();

private:
	void UpdateMeshInfoBuffer();

private:
	Graphics::RenderDevice* device_ = nullptr;
	std::unordered_map<Entity, EmitterState> emitters_;

	std::unique_ptr<Graphics::StructuredBuffer> meshInfoBuffer_; // 追加

	// Compute Shader 用
	struct UpdateParams {
		float dt;
		Engine::Math::Vector3 emitterPos;
		uint32_t totalParticles;
		float seed;
		uint32_t modelIndex;
		uint32_t textureIndex; // 追加
	};
	std::unique_ptr<Graphics::ConstantBuffer> updateCB_;
	float time_ = 0.0f; // シード用
};

} // namespace Engine::ECS
