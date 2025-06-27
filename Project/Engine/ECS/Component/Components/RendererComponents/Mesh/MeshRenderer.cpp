#include "MeshRenderer.h"

/// engine
#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

MeshRenderer::MeshRenderer() {
	SetMeshPath("./Assets/Models/primitive/cube.obj");
	SetTexturePath("./Packages/Textures/white.png");
}

MeshRenderer::~MeshRenderer() {}


MonoString* InternalGetMeshName(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return nullptr;
	}

	return mono_string_new(mono_domain_get(), renderer->GetMeshPath().c_str());
}

void InternalSetMeshName(uint64_t _nativeHandle, MonoString* _meshName) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return;
	}

	/// stringに変換&設定
	std::string meshName = mono_string_to_utf8(_meshName);
	renderer->SetMeshPath(meshName);
}

Vector4 InternalGetMeshColor(uint64_t _nativeHandle) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (!renderer) {
		Console::Log("MeshRenderer pointer is null");
		return Vector4::kZero;
	}

	return renderer->GetColor();
}

void InternalSetMeshColor(uint64_t _nativeHandle, Vector4 _color) {
	/// ptrから MeshRenderer を取得
	MeshRenderer* renderer = reinterpret_cast<MeshRenderer*>(_nativeHandle);
	if (renderer) {
		renderer->SetColor(_color);
	} else {
		Console::Log("MeshRenderer pointer is null");
	}
}
