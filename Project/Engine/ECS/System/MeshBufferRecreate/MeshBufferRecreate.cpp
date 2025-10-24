#include "MeshBufferRecreate.h"

/// engine
#include "../../EntityComponentSystem/EntityComponentSystem.h"
#include "../../Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"

MeshBufferRecreate::MeshBufferRecreate(DxDevice* _dxDevice) : pDxDevice_(_dxDevice) {}

void MeshBufferRecreate::RuntimeUpdate(ECSGroup* _ecs) {
	/// ----- MeshRendererでBufferの再生成要求が出た場合Bufferを生成しなおす処理 ----- ///

	ComponentArray<CustomMeshRenderer>* meshRendererArray = _ecs->GetComponentArray<CustomMeshRenderer>();
	if (!meshRendererArray || meshRendererArray->GetUsedComponents().empty()) {
		return; // メッシュレンダラーが存在しない場合は何もしない
	}

	for (auto& meshRenderer : meshRendererArray->GetUsedComponents()) {
		/// 以降の処理をしない条件。[ポインタの無効/インスタンスの無効化]
		if (!meshRenderer || !meshRenderer->enable) {
			continue;
		}

		meshRenderer->VertexMemcpy();

		if (meshRenderer->GetIsBufferRecreate()) {
			meshRenderer->MeshRecreate(pDxDevice_);
			meshRenderer->SetIsBufferRecreate(false);
		}
	}

}
