#include "ComponentApplyFunc.h"

/// std
#include <unordered_map>
#include <memory>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// components
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/Script/MonoScriptEngine.h"


using namespace ONEngine;

namespace {

std::unordered_map<MonoClass*, ComponentApplyFunc> gApplyFuncMap;
std::unordered_map<MonoClass*, ComponentFetchFunc> gFetchFuncMap;
std::unordered_map<MonoClass*, size_t> gComponentBatchSize;


struct TransformBatch {
	uint32_t compId;
	Vector3 position;
	Quaternion rotate;
	Vector3 scale;
};


struct MeshRendererBatch {
	uint32_t compId;
	std::string meshPath;
	Vector4 color;
	uint32_t postEffectFlags;
};

} /// unnamed namespace


void ComponentApplyFuncs::ApplyTransform(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<TransformBatch*>(_element);
	auto* tArray = _ecsGroup->GetComponentArray<Transform>();
	if(!CheckComponentArrayEnable(tArray)) {
		return;
	}

	if(Transform* t = tArray->GetComponent(data->compId)) {
		t->SetPosition(data->position);
		t->SetRotate(data->rotate);
		t->SetScale(data->scale);
		t->Update();
	}
}

void ComponentApplyFuncs::ApplyMeshRenderer(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<MeshRendererBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<MeshRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	//if(MeshRenderer* mr = array->GetComponent(data->compId)) {

	//}
}

void ONEngine::ComponentApplyFuncs::FetchTransform(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<TransformBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<Transform>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(Transform* t = array->GetComponent(data->compId)) {
		data->position = t->GetPosition();
		data->rotate = t->GetRotate();
		data->scale = t->GetScale();
	}
}

void ONEngine::ComponentApplyFuncs::FetchMeshRenderer(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<MeshRendererBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<MeshRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	//if(MeshRenderer* mr = array->GetComponent(data->compId)) {

	//}
}

ComponentApplyFunc ComponentApplyFuncs::GetApplyFunc(MonoClass* _monoClass) {
	auto itr = gApplyFuncMap.find(_monoClass);
	if(itr == gApplyFuncMap.end()) {
		return nullptr;
	}
	return itr->second;
}

ComponentFetchFunc ONEngine::ComponentApplyFuncs::GetFetchFunc(MonoClass* _monoClass) {
	auto itr = gFetchFuncMap.find(_monoClass);
	if(itr == gFetchFuncMap.end()) {
		return nullptr;
	}
	return itr->second;
}

size_t ONEngine::ComponentApplyFuncs::GetBatchElementSize(MonoClass* _monoClass) {
	auto itr = gComponentBatchSize.find(_monoClass);
	if(itr == gComponentBatchSize.end()) {
		return 0;
	}
	return itr->second;
}

void ONEngine::ComponentApplyFuncs::Initialize(MonoImage* _monoImage) {
	{	/// Transform
		MonoClass* monoClass = mono_class_from_name(_monoImage, "", "Transform");
		gApplyFuncMap[monoClass] = ApplyTransform;
		gFetchFuncMap[monoClass] = FetchTransform;
		gComponentBatchSize[monoClass] = sizeof(TransformBatch);
	}

	{	/// MeshRenderer
		MonoClass* monoClass = mono_class_from_name(_monoImage, "", "MeshRenderer");
		gApplyFuncMap[monoClass] = ApplyMeshRenderer;
		gFetchFuncMap[monoClass] = FetchMeshRenderer;
		gComponentBatchSize[monoClass] = sizeof(MeshRendererBatch);
	}
}
