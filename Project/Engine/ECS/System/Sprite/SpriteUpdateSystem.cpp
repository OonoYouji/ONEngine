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



}

void SpriteUpdateSystem::RuntimeUpdate(ECSGroup* /*_ecs*/) {

}
