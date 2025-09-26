#include "SpriteUpdateSystem.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"


void SpriteUpdateSystem::OutsideOfRuntimeUpdate(ECSGroup* _ecs) {
	ComponentArray<SpriteRenderer>* spriteRendererArray = _ecs->GetComponentArray<SpriteRenderer>();
	if (!spriteRendererArray || spriteRendererArray->GetUsedComponents().empty()) {
		return;
	}

	/// texture idの更新を行う
	for(auto& spriteRenderer : spriteRendererArray->GetUsedComponents()) {
		/// texture idからtextureを得る
		Material& material = spriteRenderer->GetMaterial();
		material.entityId = spriteRenderer->GetOwner()->GetId();
		material.postEffectFlags = 0;
		material.normalTextureId = -1;
	}


}

void SpriteUpdateSystem::RuntimeUpdate(ECSGroup* _ecs) {

}
