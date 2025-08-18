#include "Transform.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"


Transform::Transform() {
	position = Vector3::kZero;
	rotate = Quaternion::kIdentity;
	scale = Vector3::kOne;
}
Transform::~Transform() {}


void Transform::Update() {
	matWorld = Matrix4x4::MakeScale(scale) * Matrix4x4::MakeRotate(Quaternion::Normalize(rotate)) * Matrix4x4::MakeTranslate(position);
}

void Transform::SetPosition(const Vector3& _v) {
	position = _v;
}

void Transform::SetPositionX(float _x) {
	position.x = _x;
}

void Transform::SetPositionY(float _y) {
	position.y = _y;
}

void Transform::SetPositionZ(float _z) {
	position.z = _z;
}

void Transform::SetRotate(const Vector3& _v) {
	rotate = Quaternion::FromEuler(_v);
}

void Transform::SetRotate(const Quaternion& _q) {
	rotate = _q;
}

void Transform::SetScale(const Vector3& _v) {
	scale = _v;
}

void Transform::SetScaleX(float _x) {
	scale.x = _x;
}

void Transform::SetScaleY(float _y) {
	scale.y = _y;
}

void Transform::SetScaleZ(float _z) {
	scale.z = _z;
}

const Vector3& Transform::GetPosition() const {
	return position;
}

const Quaternion& Transform::GetRotate() const {
	return rotate;
}

const Vector3& Transform::GetScale() const {
	return scale;
}

const Matrix4x4& Transform::GetMatWorld() const {
	return matWorld;
}


/// ===================================================
/// mono からのTransform取得用関数
/// ===================================================

void UpdateTransform(Transform* _transform) {
	if (GameEntity* entity = _transform->GetOwner()) {
		entity->UpdateTransform();
	}
}

void InternalGetPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	Vector3 position = Matrix4x4::Transform({}, transform->matWorld);

	if (_x) { *_x = position.x; }
	if (_y) { *_y = position.y; }
	if (_z) { *_z = position.z; }
}

void InternalGetLocalPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	if (_x) { *_x = transform->position.x; }
	if (_y) { *_y = transform->position.y; }
	if (_z) { *_z = transform->position.z; }
}

void InternalGetRotate(uint64_t _nativeHandle, float* _x, float* _y, float* _z, float* _w) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	if (_x) { *_x = transform->rotate.x; }
	if (_y) { *_y = transform->rotate.y; }
	if (_z) { *_z = transform->rotate.z; }
	if (_w) { *_w = transform->rotate.w; } 
}

void InternalGetScale(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	if (_x) { *_x = transform->scale.x; }
	if (_y) { *_y = transform->scale.y; }
	if (_z) { *_z = transform->scale.z; }
}

void InternalSetPosition(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->position.x = _x;
	transform->position.y = _y;
	transform->position.z = _z;
	UpdateTransform(transform); // 更新を呼び出す
}

void InternalSetLocalPosition(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->position.x = _x;
	transform->position.y = _y;
	transform->position.z = _z;
	UpdateTransform(transform); // 更新を呼び出す
}

void InternalSetRotate(uint64_t _nativeHandle, float _x, float _y, float _z, float _w) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->rotate.x = _x;
	transform->rotate.y = _y;
	transform->rotate.z = _z;
	transform->rotate.w = _w;
	UpdateTransform(transform); // 更新を呼び出す
}

void InternalSetScale(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if (!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->scale.x = _x;
	transform->scale.y = _y;
	transform->scale.z = _z;
	UpdateTransform(transform); // 更新を呼び出す
}

void COMP_DEBUG::TransformDebug(Transform* _transform) {
	if (!_transform) {
		return;
	}

	bool isEdit = false;

	isEdit |= ImGui::DragFloat3("position", &_transform->position.x, 0.1f);
	isEdit |= ImGui::DragFloat3("rotate", &_transform->rotate.x, Mathf::PI / 12.0f);
	isEdit |= ImGui::DragFloat3("scale", &_transform->scale.x, 0.1f);

	if (isEdit) {
		_transform->Update();
	}

}


void from_json(const nlohmann::json& _j, Transform& _t) {
	_t.enable = _j.at("enable").get<int>();
	_t.position = _j.at("position").get<Vector3>();
	_t.rotate = _j.at("rotate").get<Quaternion>();
	_t.scale = _j.at("scale").get<Vector3>();
	_t.Update(); // 初期化時に更新を呼び出す
}

void to_json(nlohmann::json& _j, const Transform& _t) {
	_j = nlohmann::json{
		{ "type", "Transform" },
		{ "enable", _t.enable },
		{ "position", _t.position },
		{ "rotate", _t.rotate },
		{ "scale", _t.scale }
	};
}
