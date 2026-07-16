#include "ComponentApplyFunc.h"

/// std
#include <unordered_map>
#include <memory>

/// engine
#include "Engine/Core/Utility/Utility.h"

/// components
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Agent/AgentIntentComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animator/Animator.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/DissolveMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/Script/MonoScriptEngine.h"


#include "Engine/Graphics/Buffer/Data/UVTransform.h"

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
	Matrix4x4 matWorld;
};

struct MeshRendererBatch {
	uint32_t compId;
	Vector4 color;
	uint32_t postEffectFlags;
	UVTransform uvTransform;
};

struct DissolveBatch {
	uint32_t compId;
	float threshold;
	UVTransform uvTransform;
};

struct SpriteBatch {
	uint32_t compId;
	Vector4 color;
	Vector2 textureSize;
	UVTransform uvTransform;
};

struct CameraBatch {
	uint32_t compId;
	Matrix4x4 matVP;
	Matrix4x4 matView;
	Matrix4x4 matProjection;
	float fovY;
	float nearClip;
	float farClip;
	int cameraType;
};

struct AnimatorBatch {
    uint32_t compId;
    AnimationLayer layers[MAX_ANIMATION_LAYERS];
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
		// matWorldはC++側で再計算されるためC#からの値で上書きしない
	}
}

void ComponentApplyFuncs::ApplyMeshRenderer(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<MeshRendererBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<MeshRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(MeshRenderer* mr = array->GetComponent(data->compId)) {
		mr->SetColor(data->color);
		mr->SetPostEffectFlags(data->postEffectFlags);
		mr->SetUVTransform(data->uvTransform);
	}
}

void ONEngine::ComponentApplyFuncs::ApplyDissolve(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<DissolveBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<DissolveMeshRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(DissolveMeshRenderer* mr = array->GetComponent(data->compId)) {
		mr->SetThreshold(data->threshold);
		mr->SetUVTransform(data->uvTransform);
	}
}

void ONEngine::ComponentApplyFuncs::ApplySprite(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<SpriteBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<SpriteRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(SpriteRenderer* sr = array->GetComponent(data->compId)) {
		sr->SetColor(data->color);
		sr->SetUVTransform(data->uvTransform);
	}
}

void ONEngine::ComponentApplyFuncs::ApplyAgentIntent(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<AgentIntentComponent::BatchData*>(_element);
	auto* array = _ecsGroup->GetComponentArray<AgentIntentComponent>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(AgentIntentComponent* ai = array->GetComponent(data->compId)) {
		ai->desiredMoveDirection = data->desiredMoveDirection;
		ai->desiredRotation = data->desiredRotation;
		ai->rotationSpeed = data->rotationSpeed;
		ai->maxSpeed = data->maxSpeed;
		ai->useDesiredRotation = (data->useDesiredRotation != 0);
		ai->isAttacking = (data->isAttacking != 0);
		ai->targetEntityId = data->targetEntityId;
	}
}

void ONEngine::ComponentApplyFuncs::ApplyCamera(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<CameraBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<CameraComponent>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(CameraComponent* camera = array->GetComponent(data->compId)) {
		// matView と matProjection は現状 C++ 側で計算されていることが多いが、
		// C# 側から上書きしたい場合のために実装しておく
		// ただし、UpdateViewProjection で上書きされる可能性があるので注意

		camera->fovY_ = data->fovY;
		camera->nearClip_ = data->nearClip;
		camera->farClip_ = data->farClip;
		camera->cameraType_ = data->cameraType;
	}
}

void ONEngine::ComponentApplyFuncs::ApplyAnimator(void* _element, ECSGroup* _ecsGroup) {
    auto* data = static_cast<AnimatorBatch*>(_element);
    auto* array = _ecsGroup->GetComponentArray<Animator>();
    if (!CheckComponentArrayEnable(array)) return;

    if (Animator* animator = array->GetComponent(data->compId)) {
        for (uint32_t i = 0; i < MAX_ANIMATION_LAYERS; ++i) {
            animator->layers[i] = data->layers[i];
        }
    }
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
		data->matWorld = t->GetMatWorld();
	} else {
		Console::LogWarning("[SYNC_ERROR] FetchTransform: Component with ID " + std::to_string(data->compId) + " not found in group " + _ecsGroup->GetGroupName());
	}
}

void ONEngine::ComponentApplyFuncs::FetchMeshRenderer(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<MeshRendererBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<MeshRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(MeshRenderer* mr = array->GetComponent(data->compId)) {
		data->color = mr->GetColor();
		data->postEffectFlags = mr->GetPostEffectFlags();
		data->uvTransform = mr->GetUVTransform();
	}
}

void ONEngine::ComponentApplyFuncs::FetchDissolve(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<DissolveBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<DissolveMeshRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(DissolveMeshRenderer* mr = array->GetComponent(data->compId)) {
		data->threshold = mr->GetDissolveThreshold();
		data->compId = mr->GetDissolveCompare();
		data->uvTransform = mr->GetUVTransform();
	}
}

void ONEngine::ComponentApplyFuncs::FetchSprite(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<SpriteBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<SpriteRenderer>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(SpriteRenderer* sr = array->GetComponent(data->compId)) {
		data->color = sr->GetColor();
		data->textureSize = sr->GetTextureSize(Asset::AssetCollection::GetInstance());
		data->uvTransform = sr->GetUVTransform();
	}
}

void ONEngine::ComponentApplyFuncs::FetchAgentIntent(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<AgentIntentComponent::BatchData*>(_element);
	auto* array = _ecsGroup->GetComponentArray<AgentIntentComponent>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(AgentIntentComponent* ai = array->GetComponent(data->compId)) {
		data->desiredMoveDirection = ai->desiredMoveDirection;
		data->desiredRotation = ai->desiredRotation;
		data->rotationSpeed = ai->rotationSpeed;
		data->maxSpeed = ai->maxSpeed;
		data->useDesiredRotation = ai->useDesiredRotation;
		data->isAttacking = ai->isAttacking;
		data->targetEntityId = ai->targetEntityId;
	} else {
		Console::LogWarning("[SYNC_ERROR] FetchAgentIntent: Component with ID " + std::to_string(data->compId) + " not found in group " + _ecsGroup->GetGroupName());
	}
}

void ONEngine::ComponentApplyFuncs::FetchCamera(void* _element, ECSGroup* _ecsGroup) {
	auto* data = static_cast<CameraBatch*>(_element);
	auto* array = _ecsGroup->GetComponentArray<CameraComponent>();
	if(!CheckComponentArrayEnable(array)) {
		return;
	}

	if(CameraComponent* camera = array->GetComponent(data->compId)) {
		if (camera->IsMakeViewProjection()) {
			const ViewProjection& vp = camera->GetViewProjection();
			data->matVP = vp.matVP;
			data->matView = vp.matView;
			data->matProjection = vp.matProjection;
		}
		else {
			data->matVP = Matrix4x4::kIdentity;
			data->matView = Matrix4x4::kIdentity;
			data->matProjection = Matrix4x4::kIdentity;
		}

		data->fovY = camera->fovY_;
		data->nearClip = camera->nearClip_;
		data->farClip = camera->farClip_;
		data->cameraType = camera->cameraType_;
	}
}

void ONEngine::ComponentApplyFuncs::FetchAnimator(void* _element, ECSGroup* _ecsGroup) {
    auto* data = static_cast<AnimatorBatch*>(_element);
    auto* array = _ecsGroup->GetComponentArray<Animator>();
    if (!CheckComponentArrayEnable(array)) return;

    if (Animator* animator = array->GetComponent(data->compId)) {
        for (uint32_t i = 0; i < MAX_ANIMATION_LAYERS; ++i) {
            data->layers[i] = animator->layers[i];
        }
    }
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
	gApplyFuncMap.clear();
	gFetchFuncMap.clear();
	gComponentBatchSize.clear();

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

	{	/// DissolveMeshRenderer
		MonoClass* monoClass = mono_class_from_name(_monoImage, "", "DissolveMeshRenderer");
		gApplyFuncMap[monoClass] = ApplyDissolve;
		gFetchFuncMap[monoClass] = FetchDissolve;
		gComponentBatchSize[monoClass] = sizeof(DissolveBatch);
	}

	{	/// SpriteRenderer
		MonoClass* monoClass = mono_class_from_name(_monoImage, "", "SpriteRenderer");
		gApplyFuncMap[monoClass] = ApplySprite;
		gFetchFuncMap[monoClass] = FetchSprite;
		gComponentBatchSize[monoClass] = sizeof(SpriteBatch);
	}

	{	/// AgentIntentComponent
		MonoClass* monoClass = mono_class_from_name(_monoImage, "", "AgentIntentComponent");
		if (monoClass) {
			gApplyFuncMap[monoClass] = ApplyAgentIntent;
			gFetchFuncMap[monoClass] = FetchAgentIntent;
			gComponentBatchSize[monoClass] = sizeof(AgentIntentComponent::BatchData);
		}
	}

	{	/// CameraComponent
		MonoClass* monoClass = mono_class_from_name(_monoImage, "", "CameraComponent");
		if (monoClass) {
			gApplyFuncMap[monoClass] = ApplyCamera;
			gFetchFuncMap[monoClass] = FetchCamera;
			gComponentBatchSize[monoClass] = sizeof(CameraBatch);
		}
	}

	{	/// Animator
		MonoClass* monoClass = mono_class_from_name(_monoImage, "", "Animator");
		if (monoClass) {
			gApplyFuncMap[monoClass] = ApplyAnimator;
			gFetchFuncMap[monoClass] = FetchAnimator;
			gComponentBatchSize[monoClass] = sizeof(AnimatorBatch);
		}
	}
}
