#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/MeshRenderer.h"
#include "Engine/ECS/Components/Light.h"
#include "Engine/Graphics/Core/Renderer.h"

namespace Engine::ECS {

///
/// 描画リクエストをキューに送るシステム
///
class RenderSystem final : public System {
public:
	void Update(Registry& registry) override {
		auto& renderer = Engine::Graphics::Renderer::GetInstance();

		// メッシュの収集
		auto meshView = registry.GetView<Transform, MeshRenderer>();
		meshView.Each([&]([[maybe_unused]] Entity entity, Transform& transform, MeshRenderer& meshRenderer) {
			Engine::Graphics::RenderRequest request;
			request.meshPath = meshRenderer.meshPath;
			request.position = transform.position;
			request.rotation = transform.rotation;
			request.scale = transform.scale;
			renderer.PushRequest(request);
		});

		// 平行光源の収集
		auto dirLightView = registry.GetView<DirectionalLight>();
		dirLightView.Each([&]([[maybe_unused]] Entity entity, DirectionalLight& light) {
			renderer.AddDirectionalLight({ light.color, light.intensity, light.direction });
		});

		// 点光源の収集
		auto pointLightView = registry.GetView<Transform, PointLight>();
		pointLightView.Each([&]([[maybe_unused]] Entity entity, Transform& transform, PointLight& light) {
			renderer.AddPointLight({ transform.position, light.color, light.intensity, light.radius });
		});
	}
};

} // namespace Engine::ECS
