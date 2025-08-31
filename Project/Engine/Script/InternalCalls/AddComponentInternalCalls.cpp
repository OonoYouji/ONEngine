#include "AddComponentInternalCalls.h"

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/Core/Utility/Input/InputSystem.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"
#include "Engine/Scene/SceneManager.h"

using namespace MONO_INTERNAL_METHOD;

void AddComponentInternalCalls() {

	/// transform
	mono_add_internal_call("Transform::InternalGetPosition", (void*)InternalGetPosition);
	mono_add_internal_call("Transform::InternalGetLocalPosition", (void*)InternalGetLocalPosition);
	mono_add_internal_call("Transform::InternalGetRotate", (void*)InternalGetRotate);
	mono_add_internal_call("Transform::InternalGetScale", (void*)InternalGetScale);
	mono_add_internal_call("Transform::InternalSetPosition", (void*)InternalSetPosition);
	mono_add_internal_call("Transform::InternalSetLocalPosition", (void*)InternalSetLocalPosition);
	mono_add_internal_call("Transform::InternalSetRotate", (void*)InternalSetRotate);
	mono_add_internal_call("Transform::InternalSetScale", (void*)InternalSetScale);

	/// mesh renderer
	mono_add_internal_call("MeshRenderer::InternalGetMeshName", (void*)InternalGetMeshName);
	mono_add_internal_call("MeshRenderer::InternalSetMeshName", (void*)InternalSetMeshName);
	mono_add_internal_call("MeshRenderer::InternalGetColor", (void*)InternalGetMeshColor);
	mono_add_internal_call("MeshRenderer::InternalSetColor", (void*)InternalSetMeshColor);
	mono_add_internal_call("MeshRenderer::InternalGetPostEffectFlags", (void*)InternalGetPostEffectFlags);
	mono_add_internal_call("MeshRenderer::InternalSetPostEffectFlags", (void*)InternalSetPostEffectFlags);


	/// skin mesh renderer
	mono_add_internal_call("SkinMeshRenderer::InternalGetMeshPath", (void*)InternalGetMeshPath);
	mono_add_internal_call("SkinMeshRenderer::InternalSetMeshPath", (void*)InternalSetMeshPath);
	mono_add_internal_call("SkinMeshRenderer::InternalGetTexturePath", (void*)InternalGetTexturePath);
	mono_add_internal_call("SkinMeshRenderer::InternalSetTexturePath", (void*)InternalSetTexturePath);
	mono_add_internal_call("SkinMeshRenderer::InternalGetIsPlaying", (void*)InternalGetIsPlaying);
	mono_add_internal_call("SkinMeshRenderer::InternalSetIsPlaying", (void*)InternalSetIsPlaying);
	mono_add_internal_call("SkinMeshRenderer::InternalGetAnimationTime", (void*)InternalGetAnimationTime);
	mono_add_internal_call("SkinMeshRenderer::InternalSetAnimationTime", (void*)InternalSetAnimationTime);
	mono_add_internal_call("SkinMeshRenderer::InternalGetAnimationScale", (void*)InternalGetAnimationScale);
	mono_add_internal_call("SkinMeshRenderer::InternalSetAnimationScale", (void*)InternalSetAnimationScale);
	mono_add_internal_call("SkinMeshRenderer::InternalGetJointTransform", (void*)InternalGetJointTransform);


	/// script (MonoBehavior)
	mono_add_internal_call("MonoBehavior::InternalSetEnable", (void*)InternalSetEnable);
	mono_add_internal_call("MonoBehavior::InternalGetEnable", (void*)InternalGetEnable);

	/// audio source
	mono_add_internal_call("AudioSource::InternalGetParams", (void*)InternalGetParams);
	mono_add_internal_call("AudioSource::InternalSetParams", (void*)InternalSetParams);
	mono_add_internal_call("AudioSource::InternalPlayOneShot", (void*)InternalPlayOneShot);

}

void AddEntityInternalCalls() {
	/// entity
	mono_add_internal_call("Entity::InternalAddComponent", (void*)InternalAddComponent);
	mono_add_internal_call("Entity::InternalGetComponent", (void*)InternalGetComponent);
	mono_add_internal_call("Entity::InternalGetName", (void*)InternalGetName);
	mono_add_internal_call("Entity::InternalSetName", (void*)InternalSetName);
	mono_add_internal_call("Entity::InternalGetChildId", (void*)InternalGetChildId);
	mono_add_internal_call("Entity::InternalGetParentId", (void*)InternalGetParentId);
	mono_add_internal_call("Entity::InternalSetParent", (void*)InternalSetParent);
	mono_add_internal_call("Entity::InternalAddScript", (void*)InternalAddScript);
	mono_add_internal_call("Entity::InternalGetScript", (void*)InternalGetScript);

	mono_add_internal_call("ECSGroup::InternalCreateEntity", (void*)InternalCreateEntity);
	mono_add_internal_call("ECSGroup::InternalDestroyEntity", (void*)InternalDestroyEntity);

}

void AddInputInternalCalls() {
	mono_add_internal_call("Input::InternalTriggerKey", (void*)Input::TriggerKey);
	mono_add_internal_call("Input::InternalPressKey", (void*)Input::PressKey);
	mono_add_internal_call("Input::InternalReleaseKey", (void*)Input::ReleaseKey);

	mono_add_internal_call("Input::InternalTriggerGamepad", (void*)Input::TriggerGamepad);
	mono_add_internal_call("Input::InternalPressGamepad", (void*)Input::PressGamepad);
	mono_add_internal_call("Input::InternalReleaseGamepad", (void*)Input::ReleaseGamepad);
	mono_add_internal_call("Input::InternalGetGamepadThumb", (void*)InternalGetGamepadThumb);

	mono_add_internal_call("Input::InternalTriggerMouse", (void*)Input::TriggerMouse);
	mono_add_internal_call("Input::InternalPressMouse", (void*)Input::PressMouse);
	mono_add_internal_call("Input::InternalReleaseMouse", (void*)Input::ReleaseMouse);
	mono_add_internal_call("Input::InternalGetMousePosition", (void*)InternalGetMousePosition);
	mono_add_internal_call("Input::InternalGetMouseVelocity", (void*)InternalGetMouseVelocity);
}

void AddSceneInternalCalls() {
	mono_add_internal_call("SceneManager::InternalLoadScene", (void*)InternalLoadScene);
}
