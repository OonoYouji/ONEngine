#include "SpriteUpdateSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"


void SpriteUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {

	/// スプライトの配列、存在チェック
	ComponentArray<SpriteRenderer>* spriteRendererArray = _ecs->GetComponentArray<SpriteRenderer>();
	if (!spriteRendererArray || spriteRendererArray->GetUsedComponents().empty()) {
		return;
	}

	/// EntityIDの更新を行う(EntityIDは描画時にRTVに書き込まれる)
	for(auto& spriteRenderer : spriteRendererArray->GetUsedComponents()) {
		GPUMaterial& material = spriteRenderer->GetMaterial();
		material.entityId = spriteRenderer->GetOwner()->GetId();
	}


}

void SpriteUpdateSystem::RuntimeUpdate(ECSGroup* /*_ecs*/) {

}
