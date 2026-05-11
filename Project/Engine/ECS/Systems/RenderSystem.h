#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Registry.h"
#include "Schema/Schema.h"
#include "Engine/Graphics/Core/Renderer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Mesh.h"

namespace Engine::ECS {

///
/// 描画リクエストを収集するシステム
///
class RenderSystem : public System {
public:
	void Update(Registry& registry) override {
		auto& renderer = Engine::Graphics::Renderer::GetInstance();
		auto& am = Engine::Asset::AssetManager::GetInstance();

		// 1. メッシュレンダラーを持つエンティティを収集
		registry.GetView<Transform, MeshRenderer>().Each([&](Entity entity, Transform& transform, MeshRenderer& meshRenderer) {
			const auto& meshes = am.GetMeshesByIndex(meshRenderer.modelIndex);
			
			uint32_t subIdx = 0;
			for (const auto& mesh : meshes) {
				Engine::Graphics::RenderRequest request;
				request.modelIndex = meshRenderer.modelIndex;
				request.materialIndex = meshRenderer.materialIndex;
				request.vertexOffset = mesh->GetVertexOffset();
				request.world = transform.world;
				request.entityID = static_cast<uint32_t>(entity);
				request.postProcessFlags = meshRenderer.postProcessFlags;
				request.isSkinned = false;
				request.skeletonIndex = 0;

				renderer.PushRequest(request);
			}
		});

		// 2. スキニングメッシュレンダラーを持つエンティティを収集
		registry.GetView<Transform, SkinnedMeshRenderer>().Each([&](Entity entity, Transform& transform, SkinnedMeshRenderer& skinnedRenderer) {
			const auto& meshes = am.GetMeshesByIndex(skinnedRenderer.modelIndex);
			
			uint32_t currentSubMeshOffset = 0;
			uint32_t subIdx = 0;
			for (const auto& mesh : meshes) {
				Engine::Graphics::RenderRequest request;
				request.modelIndex = skinnedRenderer.modelIndex;
				request.materialIndex = skinnedRenderer.materialIndex;
				
				// AnimationSystem で割り当てられた動的オフセットを使用
				// (全メッシュが連続して stack に積まれている前提)
				request.vertexOffset = skinnedRenderer.internalVertexOffset + currentSubMeshOffset;
				currentSubMeshOffset += mesh->GetVertexCount();

				request.world = transform.world;
				request.entityID = static_cast<uint32_t>(entity);
				request.postProcessFlags = skinnedRenderer.postProcessFlags;
				request.isSkinned = true;
				request.skeletonIndex = skinnedRenderer.skeletonIndex;

				renderer.PushRequest(request);
			}
		});
	}
};

} // namespace Engine::ECS
