#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/MeshRenderer.h"
#include "Engine/Graphics/Core/Renderer.h"

namespace Engine::ECS {

///
/// 描画リクエストを収集してRendererに送るシステム
///
class RenderSystem final : public System {
public:
	void Update(Registry& registry) override {
		auto& renderer = Engine::Graphics::Renderer::GetInstance();

		// メッシュレンダラーを持つエンティティを収集
		registry.GetView<Transform, MeshRenderer>().Each([&](Entity entity, Transform& transform, MeshRenderer& meshRenderer) {
			Engine::Graphics::RenderRequest request;
			request.modelName = meshRenderer.modelName;
			request.materialName = meshRenderer.materialName;
			
			// Transformから行列を計算
			request.world = Engine::Math::Matrix4x4::MakeAffine(transform.scale, transform.rotation, transform.position);
			
			renderer.PushRequest(request);
		});
	}
};

} // namespace Engine::ECS
