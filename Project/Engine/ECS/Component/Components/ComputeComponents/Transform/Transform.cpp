#include "Transform.h"
#include <nlohmann/json.hpp>

#define NOMINMAX

/// std
#include <limits>
#include <vector>

/// externals
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"

/// editor
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentJsonConverter.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ModifyTransformCommand.h"
#include "Engine/Editor/EditorUtils.h"
#include "Engine/Editor/Manager/EditCommand.h"

using namespace ONEngine;

Transform::Transform() {
	position = Vector3::Zero;
	rotate = Quaternion::kIdentity;
	scale = Vector3::One;
	euler = Vector3::Zero;
	lastSyncedRotate = Quaternion::kIdentity;
}
Transform::~Transform() = default;


void Transform::Update() {
	matWorld = Matrix4x4::MakeScale(scale) * Matrix4x4::MakeRotate(Quaternion::Normalize(rotate)) * Matrix4x4::MakeTranslate(position);
}

void Transform::Reset() {
	position = Vector3::Zero;
	rotate = Quaternion::kIdentity;
	scale = Vector3::One;
	euler = Vector3::Zero;
	lastSyncedRotate = Quaternion::kIdentity;
	matWorld = Matrix4x4::kIdentity;
}

void Transform::SetPosition(const Vector3& _v) {
	position = _v;
}

void Transform::SetRotate(const Vector3& _v) {
	euler = _v; // ここでの _v は度数法を想定
	SyncQuaternionFromEuler();
}

void Transform::SetRotate(const Quaternion& _q) {
	rotate = _q;
	SyncEulerFromQuaternion();
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

void Transform::SyncQuaternionFromEuler() {
	Vector3 rad = {
		euler.x * Math::Deg2Rad,
		euler.y * Math::Deg2Rad,
		euler.z * Math::Deg2Rad
	};
	rotate = Quaternion::FromEuler(rad);
	lastSyncedRotate = rotate;
}

void Transform::SyncEulerFromQuaternion() {
	Vector3 rad = Quaternion::ToEuler(rotate);
	euler = {
		rad.x * Math::Rad2Deg,
		rad.y * Math::Rad2Deg,
		rad.z * Math::Rad2Deg
	};
	lastSyncedRotate = rotate;
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
	transform->SyncEulerFromQuaternion(); // Eulerキャッシュを同期
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
	std::vector<Transform*> transforms = { _transform };
	TransformDebug(transforms);
}

void ComponentDebug::TransformDebug(const std::vector<Transform*>& _transforms) {
	if(_transforms.empty()) {
		return;
	}

	Transform* first = _transforms[0];

	// アニメーションなどで外部から回転が書き換えられていないかチェック
	if (!(first->rotate == first->lastSyncedRotate)) {
		first->SyncEulerFromQuaternion();
	}

	Vector3   pos = first->position;
	Vector3   euler = first->euler; 
	Vector3   scale = first->scale;
	int       flags = first->matrixCalcFlags;

	static std::vector<Vector3> s_startPos, s_startEuler, s_startScale;

	static bool isUnifieds[3] = { false, false, true };
	constexpr float minValue = (std::numeric_limits<float>::lowest)();
	constexpr float maxValue = (std::numeric_limits<float>::max)();

	// --- Position ---
	bool posActivated = false;
	bool posDeactivated = false;
	bool posChanged = Editor::DrawVec3Control("position", pos, 0.1f, minValue, maxValue, 100.0f, &isUnifieds[0], false, &posActivated, &posDeactivated);

	if (posActivated) {
		s_startPos.clear();
		for (auto t : _transforms) s_startPos.push_back(t->position);
	}

	if (posChanged) {
		for (auto t : _transforms) { t->position = pos; t->Update(); }
	}

	if (posDeactivated) {
		if (s_startPos.size() == _transforms.size()) {
			bool changed = false;
			for (size_t i = 0; i < _transforms.size(); ++i) {
				if (s_startPos[i] != _transforms[i]->position) { changed = true; break; }
			}

			if (changed) {
				std::vector<Editor::ModifyTransformCommand::Data> data;
				for (size_t i = 0; i < _transforms.size(); ++i) {
					data.push_back({ _transforms[i], s_startPos[i], _transforms[i]->position });
				}
				Editor::EditCommand::Execute<Editor::ModifyTransformCommand>(Editor::ModifyTransformCommand::Target::Position, data);
			}
		}
	}

	// --- Rotation ---
	bool rotActivated = false;
	bool rotDeactivated = false;
	bool rotChanged = Editor::DrawVec3Control("rotation", euler, 0.5f, minValue, maxValue, 100.0f, &isUnifieds[1], false, &rotActivated, &rotDeactivated);

	if (rotActivated) {
		s_startEuler.clear();
		for (auto t : _transforms) s_startEuler.push_back(t->euler);
	}

	if (rotChanged) {
		for (auto t : _transforms) { 
			t->euler = euler; 
			t->SyncQuaternionFromEuler(); 
			t->Update(); 
		}
	}

	if (rotDeactivated) {
		if (s_startEuler.size() == _transforms.size()) {
			bool changed = false;
			for (size_t i = 0; i < _transforms.size(); ++i) {
				if (s_startEuler[i] != _transforms[i]->euler) { changed = true; break; }
			}

			if (changed) {
				std::vector<Editor::ModifyTransformCommand::Data> data;
				for (size_t i = 0; i < _transforms.size(); ++i) {
					data.push_back({ _transforms[i], s_startEuler[i], _transforms[i]->euler });
				}
				Editor::EditCommand::Execute<Editor::ModifyTransformCommand>(Editor::ModifyTransformCommand::Target::Rotation, data);
			}
		}
	}

	// --- Scale ---
	bool scaleActivated = false;
	bool scaleDeactivated = false;
	bool scaleChanged = Editor::DrawVec3Control("scale", scale, 0.1f, minValue, maxValue, 100.0f, &isUnifieds[2], false, &scaleActivated, &scaleDeactivated);

	if (scaleActivated) {
		s_startScale.clear();
		for (auto t : _transforms) s_startScale.push_back(t->scale);
	}

	if (scaleChanged) {
		for (auto t : _transforms) { t->scale = scale; t->Update(); }
	}

	if (scaleDeactivated) {
		if (s_startScale.size() == _transforms.size()) {
			bool changed = false;
			for (size_t i = 0; i < _transforms.size(); ++i) {
				if (s_startScale[i] != _transforms[i]->scale) { changed = true; break; }
			}

			if (changed) {
				std::vector<Editor::ModifyTransformCommand::Data> data;
				for (size_t i = 0; i < _transforms.size(); ++i) {
					data.push_back({ _transforms[i], s_startScale[i], _transforms[i]->scale });
				}
				Editor::EditCommand::Execute<Editor::ModifyTransformCommand>(Editor::ModifyTransformCommand::Target::Scale, data);
			}
		}
	}

	bool flagsChanged = false;
	flagsChanged |= ImGui::CheckboxFlags("matrixCalcFlags: position", &flags, Transform::kPosition);
	flagsChanged |= ImGui::CheckboxFlags("matrixCalcFlags: rotate", &flags, Transform::kRotate);
	flagsChanged |= ImGui::CheckboxFlags("matrixCalcFlags: scale", &flags, Transform::kScale);
	
	if (flagsChanged) {
		for (auto t : _transforms) { t->matrixCalcFlags = flags; t->Update(); }
	}

	first->lastSyncedRotate = first->rotate;
}


void ONEngine::from_json(const nlohmann::json& _j, Transform& _t) {
	_t.enable = _j.at("enable").get<int>();
	_t.position = _j.at("position").get<Vector3>();
	_t.rotate = _j.at("rotate").get<Quaternion>();
	_t.scale = _j.at("scale").get<Vector3>();
	_t.matrixCalcFlags = _j.value("matrixCalcFlags", Transform::kAll);
	_t.SyncEulerFromQuaternion(); 
	_t.Update();
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
