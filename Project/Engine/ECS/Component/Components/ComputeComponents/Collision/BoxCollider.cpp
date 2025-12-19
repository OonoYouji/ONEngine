#include "BoxCollider.h"

#include <magic_enum/magic_enum.hpp>

/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/EditorUtils.h"


using namespace ONEngine;

#ifdef DEBUG_MODE
void ComponentDebug::BoxColliderDebug(BoxCollider* _bc) {
	if (!_bc) {
		return;
	}

	/// 共通パラメータ
	ImGui::SeparatorText("common parameter");
	int currentIndex = static_cast<int>(_bc->collisionState_);

	auto names = magic_enum::enum_names<CollisionState>();
	std::vector<const char*> items;
	for (auto& n : names) items.push_back(n.data());

	if (ImGui::Combo("CollisionState", &currentIndex, items.data(), static_cast<int>(items.size()))) {
		_bc->collisionState_ = static_cast<CollisionState>(currentIndex);
	}


	/// box parameter
	ImGui::SeparatorText("box parameter");
	Editor::DrawVec3Control("size", _bc->size_, 0.1f, 0.0f, 1024.0f, 100.0f, &_bc->dUnified_);
}
#endif

void ONEngine::from_json(const nlohmann::json& _j, BoxCollider& _b) {
	_b.enable = _j.value("enable", 1);
	_b.size_ = _j.value("size", Vector3::One);
	_b.collisionState_ = magic_enum::enum_cast<CollisionState>(_j.value("state", "Dynamic")).value_or(CollisionState::Dynamic);
}

void ONEngine::to_json(nlohmann::json& _j, const BoxCollider& _b) {
	_j = nlohmann::json{
		{ "type", "BoxCollider" },
		{ "enable", _b.enable },
		{ "size", _b.size_ },
		{ "state", magic_enum::enum_name(_b.collisionState_) }
	};
}


BoxCollider::BoxCollider() {
	// デフォルトの値をセット
	size_ = Vector3::One; // サイズを1x1x1に初期化
}

void BoxCollider::SetSize(const Vector3& _size) {
	size_ = _size;
}

const Vector3& BoxCollider::GetSize() const {
	return size_;
}


