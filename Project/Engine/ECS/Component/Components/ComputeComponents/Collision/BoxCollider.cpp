#include "BoxCollider.h"

#include <magic_enum/magic_enum.hpp>

/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/EditorUtils.h"


using namespace ONEngine;

void ComponentDebug::BoxColliderDebug(BoxCollider* _bc) {
	if(!_bc) {
		return;
	}

	/// 共通パラメータ
	ImGui::SeparatorText("common parameter");

	{	/// CollisionState
		int currentIndex = static_cast<int>(_bc->collisionState_);
		auto names = magic_enum::enum_names<CollisionState>();
		std::vector<const char*> items;
		for(auto& n : names) items.push_back(n.data());

		if(ImGui::Combo("CollisionState", &currentIndex, items.data(), static_cast<int>(items.size()))) {
			_bc->collisionState_ = static_cast<CollisionState>(currentIndex);
		}

		ImGui::Checkbox("Is Trigger", &_bc->isTrigger_);
		ImGui::Checkbox("Freeze Y", &_bc->freezeY_);
		ImGui::DragFloat("Mass", &_bc->mass_, 0.1f, 0.001f, 10000.0f);
	}

	{	/// CollisionFilter (ImGui側の処理)
		constexpr auto entries = magic_enum::enum_entries<CollisionFilter>();
		std::vector<const char*> items;
		for(const auto& entry : entries) {
			items.push_back(entry.second.data());
		}

		ImGui::Text("Collision Settings");
		ImGui::Separator();

		// ---------------------------------------------------------
		// 1. Category の設定 (Combo)
		// ---------------------------------------------------------
		uint32_t currentCategoryBit = _bc->GetCategoryBits();
		int categoryIdx = 0;

		for(size_t i = 0; i < entries.size(); ++i) {
			if(currentCategoryBit == static_cast<uint32_t>(entries[i].first)) {
				categoryIdx = static_cast<int>(i);
				break;
			}
		}

		if(ImGui::Combo("Category", &categoryIdx, items.data(), static_cast<int>(items.size()))) {
			_bc->SetCategoryBits(static_cast<uint32_t>(entries[categoryIdx].first));
		}

		// ---------------------------------------------------------
		// 2. Mask の設定 (CheckboxFlags)
		// ---------------------------------------------------------
		ImGui::Spacing();
		ImGui::Text("Collides With (Mask):");

		uint32_t currentMask = _bc->GetMaskBits();

		if(ImGui::Button("Select All")) {
			currentMask = 0xFFFFFFFF; // ALL定数
			_bc->SetMaskBits(currentMask);
		}
		ImGui::SameLine();
		if(ImGui::Button("Clear All")) {
			currentMask = 0;
			_bc->SetMaskBits(currentMask);
		}

		// 各レイヤーをチェックボックスとして表示 (複数選択可能)
		for(const auto& entry : entries) {
			uint32_t bitValue = static_cast<uint32_t>(entry.first);
			if(ImGui::CheckboxFlags(entry.second.data(), &currentMask, bitValue)) {
				_bc->SetMaskBits(currentMask);
			}
		}
	}


	/// box parameter
	ImGui::SeparatorText("box parameter");
	static bool unified = false;
	Editor::DrawVec3Control("size", _bc->size_, 0.1f, 0.0f, 1024.0f, 100.0f, &unified);
}

void ONEngine::from_json(const nlohmann::json& _j, BoxCollider& _b) {
	_b.enable = _j.value("enable", 1);
	_b.size_ = _j.value("size", Vector3::One);
	_b.isTrigger_ = _j.value("isTrigger", false);
	_b.freezeY_ = _j.value("freezeY", false);
	_b.mass_ = _j.value("mass", 1.0f);
	_b.collisionState_ = magic_enum::enum_cast<CollisionState>(_j.value("state", "Dynamic")).value_or(CollisionState::Dynamic);
	_b.categoryBits_ = _j.value("categoryBits", static_cast<uint32_t>(CollisionFilter::Default));
	_b.maskBits_ = _j.value("maskBits", static_cast<uint32_t>(CollisionFilter::ALL));
}

void ONEngine::to_json(nlohmann::json& _j, const BoxCollider& _b) {
	_j = nlohmann::json{
		{ "type", "BoxCollider" },
		{ "enable", _b.enable },
		{ "size", _b.size_ },
		{ "isTrigger", _b.IsTrigger() },
		{ "freezeY", _b.freezeY_ },
		{ "mass", _b.mass_ },
		{ "state", magic_enum::enum_name(_b.collisionState_) },
		{ "categoryBits", _b.categoryBits_ },
		{ "maskBits", _b.maskBits_ }
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

Vector3 ONEngine::InternalGetSize(uint64_t _nativeHandle) {
	BoxCollider* c = reinterpret_cast<BoxCollider*>(_nativeHandle);
	return c ? c->GetSize() : Vector3::Zero;
}

void ONEngine::InternalSetSize(uint64_t _nativeHandle, Vector3 _size) {
	BoxCollider* c = reinterpret_cast<BoxCollider*>(_nativeHandle);
	if(c) c->SetSize(_size);
}

bool ONEngine::InternalIsTriggerBox(uint64_t _nativeHandle) {
	BoxCollider* c = reinterpret_cast<BoxCollider*>(_nativeHandle);
	return c ? c->IsTrigger() : false;
}

void ONEngine::InternalSetTriggerBox(uint64_t _nativeHandle, bool _trigger) {
	BoxCollider* c = reinterpret_cast<BoxCollider*>(_nativeHandle);
	if(c) c->SetTrigger(_trigger);
}

float ONEngine::InternalGetMassBox(uint64_t _nativeHandle) {
	BoxCollider* c = reinterpret_cast<BoxCollider*>(_nativeHandle);
	return c ? c->GetMass() : 1.0f;
}

void ONEngine::InternalSetMassBox(uint64_t _nativeHandle, float _mass) {
	BoxCollider* c = reinterpret_cast<BoxCollider*>(_nativeHandle);
	if(c) c->SetMass(_mass);
}


