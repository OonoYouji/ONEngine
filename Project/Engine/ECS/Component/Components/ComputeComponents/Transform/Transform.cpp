#include "Transform.h"

#define NOMINMAX

/// std
#include <limits>

/// externals
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"

/// editor
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Editor/EditorUtils.h"

using namespace ONEngine;

Transform::Transform() {
	position = Vector3::Zero;
	rotate = Quaternion::kIdentity;
	scale = Vector3::One;
}
Transform::~Transform() = default;


void Transform::Update() {
	matWorld = Matrix4x4::MakeScale(scale) * Matrix4x4::MakeRotate(Quaternion::Normalize(rotate)) * Matrix4x4::MakeTranslate(position);
}

void Transform::Reset() {
	position = Vector3::Zero;
	rotate = Quaternion::kIdentity;
	scale = Vector3::One;
	matWorld = Matrix4x4::kIdentity;
}

void Transform::SetPosition(const Vector3& _v) {
	position = _v;
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

void ONEngine::UpdateTransform(Transform* _transform) {
	if(GameEntity* entity = _transform->GetOwner()) {
		entity->UpdateTransform();
	}
}

void ONEngine::InternalGetPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	const Matrix4x4& matWorld = transform->GetMatWorld();
	const Vector3& position = { matWorld.m[3][0], matWorld.m[3][1], matWorld.m[3][2] };

	if(_x) { *_x = position.x; }
	if(_y) { *_y = position.y; }
	if(_z) { *_z = position.z; }
}

void ONEngine::InternalGetLocalPosition(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	if(_x) { *_x = transform->position.x; }
	if(_y) { *_y = transform->position.y; }
	if(_z) { *_z = transform->position.z; }
}

void ONEngine::InternalGetRotate(uint64_t _nativeHandle, float* _x, float* _y, float* _z, float* _w) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	if(_x) { *_x = transform->rotate.x; }
	if(_y) { *_y = transform->rotate.y; }
	if(_z) { *_z = transform->rotate.z; }
	if(_w) { *_w = transform->rotate.w; }
}

void ONEngine::InternalGetScale(uint64_t _nativeHandle, float* _x, float* _y, float* _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	if(_x) { *_x = transform->scale.x; }
	if(_y) { *_y = transform->scale.y; }
	if(_z) { *_z = transform->scale.z; }
}

void ONEngine::InternalSetPosition(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->position.x = _x;
	transform->position.y = _y;
	transform->position.z = _z;
	UpdateTransform(transform); // 更新を呼び出す
}

void ONEngine::InternalSetLocalPosition(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->position.x = _x;
	transform->position.y = _y;
	transform->position.z = _z;
	UpdateTransform(transform); // 更新を呼び出す
}

void ONEngine::InternalSetRotate(uint64_t _nativeHandle, float _x, float _y, float _z, float _w) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->rotate.x = _x;
	transform->rotate.y = _y;
	transform->rotate.z = _z;
	transform->rotate.w = _w;
	UpdateTransform(transform); // 更新を呼び出す
}

void ONEngine::InternalSetScale(uint64_t _nativeHandle, float _x, float _y, float _z) {
	Transform* transform = reinterpret_cast<Transform*>(_nativeHandle);
	if(!transform) {
		Console::LogError("Transform pointer is null");
		return;
	}

	transform->scale.x = _x;
	transform->scale.y = _y;
	transform->scale.z = _z;
	UpdateTransform(transform); // 更新を呼び出す
}

void ComponentDebug::TransformDebug(Transform* _transform) {
	if(!_transform) {
		return;
	}

	bool isEdit = false;
	static Vector3 eulerAngles = Quaternion::ToEuler(_transform->rotate);

	static bool isUnifieds[3] = { false, false, true };
	constexpr float minValue = std::numeric_limits<float>::lowest();
	constexpr float maxValue = std::numeric_limits<float>::max();
	isEdit |= Editor::ImMathf::DrawVec3Control("position", _transform->position, 0.1f,             minValue, maxValue, 100.0f, &isUnifieds[0]);
	isEdit |= Editor::ImMathf::DrawVec3Control("rotate",   eulerAngles,          Math::PI / 12.0f, minValue, maxValue, 100.0f, &isUnifieds[1]);
	isEdit |= Editor::ImMathf::DrawVec3Control("scale",    _transform->scale,    0.1f,             minValue, maxValue, 100.0f, &isUnifieds[2]);

	if(isEdit) {
		_transform->rotate = Quaternion::FromEuler(eulerAngles);
	}

	/// matrixCalcFlags 編集
	int matrixCalcFlags = _transform->matrixCalcFlags;
	isEdit |= ImGui::CheckboxFlags("matrixCalcFlags: position", &matrixCalcFlags, Transform::kPosition);
	isEdit |= ImGui::CheckboxFlags("matrixCalcFlags: rotate", &matrixCalcFlags, Transform::kRotate);
	isEdit |= ImGui::CheckboxFlags("matrixCalcFlags: scale", &matrixCalcFlags, Transform::kScale);

	_transform->matrixCalcFlags = matrixCalcFlags;

	if(isEdit) {
		_transform->Update();
	}

}


void ONEngine::from_json(const nlohmann::json& _j, Transform& _t) {
	_t.enable = _j.at("enable").get<int>();
	_t.position = _j.at("position").get<Vector3>();
	_t.rotate = _j.at("rotate").get<Quaternion>();
	_t.scale = _j.at("scale").get<Vector3>();
	_t.matrixCalcFlags = _j.value("matrixCalcFlags", Transform::kAll);
	_t.Update(); // 初期化時に更新を呼び出す
}

void ONEngine::to_json(nlohmann::json& _j, const Transform& _t) {
	_j = nlohmann::json{
		{ "type", "Transform" },
		{ "enable", _t.enable },
		{ "position", _t.position },
		{ "rotate", _t.rotate },
		{ "scale", _t.scale },
		{ "matrixCalcFlags", _t.matrixCalcFlags }
	};
}
