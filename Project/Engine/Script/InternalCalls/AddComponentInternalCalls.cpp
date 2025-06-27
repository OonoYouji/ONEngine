#include "AddComponentInternalCalls.h"

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Engine/ECS/Component/Component.h"

void AddComponentInternalCalls() {

	/// transform
	mono_add_internal_call("Transform::InternalGetTransform", (void*)InternalGetTransform);
	mono_add_internal_call("Transform::InternalGetPosition", (void*)InternalGetPosition);
	mono_add_internal_call("Transform::InternalGetRotate", (void*)InternalGetRotate);
	mono_add_internal_call("Transform::InternalGetScale", (void*)InternalGetScale);
	mono_add_internal_call("Transform::InternalSetPosition", (void*)InternalSetPosition);
	mono_add_internal_call("Transform::InternalSetRotate", (void*)InternalSetRotate);
	mono_add_internal_call("Transform::InternalSetScale", (void*)InternalSetScale);

	/// mesh renderer
	mono_add_internal_call("MeshRenderer::InternalGetMeshName", (void*)InternalGetMeshName);
	mono_add_internal_call("MeshRenderer::InternalSetMeshName", (void*)InternalSetMeshName);
	mono_add_internal_call("MeshRenderer::InternalGetColor", (void*)InternalGetMeshColor);
	mono_add_internal_call("MeshRenderer::InternalSetColor", (void*)InternalSetMeshColor);

}
