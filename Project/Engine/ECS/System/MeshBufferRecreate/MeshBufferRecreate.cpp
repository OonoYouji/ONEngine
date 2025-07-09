#include "MeshBufferRecreate.h"

/// engine
#include "../../EntityComponentSystem/EntityComponentSystem.h"
#include "../../Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"

MeshBufferRecreate::MeshBufferRecreate(DxDevice* _pDxDevice) : pDxDevice_(_pDxDevice) {}

void MeshBufferRecreate::Update([[maybe_unused]] EntityComponentSystem* _ecs, const std::vector<class IEntity*>& _entities) {

	for (auto& entity : _entities) {

		CustomMeshRenderer* meshRenderer = entity->GetComponent<CustomMeshRenderer>();

		if (!meshRenderer) {
			continue;
		}

		meshRenderer->VertexMemcpy();

		if (meshRenderer->GetIsBufferRecreate()) {
			meshRenderer->MeshRecreate(pDxDevice_);
			meshRenderer->SetIsBufferRecreate(false);
		}
	}

}
