#include "MeshBufferRecreate.h"

/// engine
#include "../../EntityComponentSystem/EntityComponentSystem.h"
#include "../../Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"

MeshBufferRecreate::MeshBufferRecreate(DxDevice* _pDxDevice) : pDxDevice_(_pDxDevice) {}

void MeshBufferRecreate::Update(EntityComponentSystem* _pEntityComponentSystem) {

	for (auto& entity : _pEntityComponentSystem->GetEntities()) {

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
