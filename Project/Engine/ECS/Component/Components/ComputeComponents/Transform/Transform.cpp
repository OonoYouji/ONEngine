#include "Transform.h"


/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"


Transform::Transform() {
	position = Vector3::kZero;
	rotate = Vector3::kZero;
	scale = Vector3::kOne;
}
Transform::~Transform() {}


void Transform::Update() {
	matWorld = Matrix4x4::MakeAffine(scale, rotate, position);
}


/// ===================================================
/// mono からのTransform取得用関数
/// ===================================================

MonoObject* InternalGetTransform(uint32_t _entityId) {
	EntityComponentSystem* ecs = GetEntityComponentSystemPtr();
	MonoScriptEngine* monoEngine = GetMonoScriptEnginePtr();

	IEntity* e = ecs->GetEntity(_entityId);
	if (!e) {
		return nullptr;
	}

	Transform* t = e->GetTransform(); // transform_ メンバを取得
	if (!t) {
		return nullptr;
	}

	MonoClass* cls = mono_class_from_name(monoEngine->Image(), "", "Transform");
	MonoObject* obj = mono_object_new(mono_domain_get(), cls);
	mono_runtime_object_init(obj);

	uint64_t ptrVal = reinterpret_cast<uint64_t>(t);
	MonoClassField* handleField = mono_class_get_field_from_name(cls, "nativeHandle");
	mono_field_set_value(obj, handleField, &ptrVal);

	return obj;
}

void InternalGetPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::Log("Transform pointer is null");
		return;
	}

	if (_x) { *_x = transform->position.x; }
	if (_y) { *_y = transform->position.y; }
	if (_z) { *_z = transform->position.z; }
}

void InternalGetRotate(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::Log("Transform pointer is null");
		return;
	}

	if (_x) { *_x = transform->rotate.x; }
	if (_y) { *_y = transform->rotate.y; }
	if (_z) { *_z = transform->rotate.z; }
}

void InternalGetScale(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::Log("Transform pointer is null");
		return;
	}

	if (_x) { *_x = transform->scale.x; }
	if (_y) { *_y = transform->scale.y; }
	if (_z) { *_z = transform->scale.z; }
}

void InternalSetPosition(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::Log("Transform pointer is null");
		return;
	}

	transform->position.x = _x;
	transform->position.y = _y;
	transform->position.z = _z;
	transform->Update(); // 更新を呼び出す
}

void InternalSetRotate(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::Log("Transform pointer is null");
		return;
	}

	transform->rotate.x = _x;
	transform->rotate.y = _y;
	transform->rotate.z = _z;
	transform->Update(); // 更新を呼び出す
}

void InternalSetScale(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::Log("Transform pointer is null");
		return;
	}

	transform->scale.x = _x;
	transform->scale.y = _y;
	transform->scale.z = _z;
	transform->Update(); // 更新を呼び出す
}

