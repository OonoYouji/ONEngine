#include "AddComponentInternalCalls.h"

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Engine/ECS/Component/Component.h"

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

}
