#include "SphereCollider.h"

/// std
#include <bit>

/// external
#include <magic_enum/magic_enum.hpp>
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"

using namespace ONEngine;

/**
 * @brief エディタ用：SphereColliderコンポーネントのデバッグ表示（Gui描画等）処理を行います。
 */
void ComponentDebug::SphereColliderDebug(SphereCollider* _c) {
	if(!_c) {
		return;
	}

	/// 共通パラメータ
	ImGui::SeparatorText("common parameter");
	{	/// CollisionState
		int currentIndex = static_cast<int>(_c->collisionState_);

		auto names = magic_enum::enum_names<CollisionState>();
		std::vector<const char*> items;
		for(auto& n : names) items.push_back(n.data());

		if(ImGui::Combo("CollisionState", &currentIndex, items.data(), (int)items.size())) {
			_c->collisionState_ = static_cast<CollisionState>(currentIndex);
		}

		ImGui::Checkbox("Is Trigger", &_c->isTrigger_);
		ImGui::Checkbox("Freeze Y", &_c->freezeY_);
		ImGui::DragFloat("Mass", &_c->mass_, 0.1f, 0.001f, 10000.0f);
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
		uint32_t currentCategoryBit = _c->GetCategoryBits();
		int categoryIdx = 0;

		for(size_t i = 0; i < entries.size(); ++i) {
			if(currentCategoryBit == static_cast<uint32_t>(entries[i].first)) {
				categoryIdx = static_cast<int>(i);
				break;
			}
		}

		if(ImGui::Combo("Category", &categoryIdx, items.data(), static_cast<int>(items.size()))) {
			_c->SetCategoryBits(static_cast<uint32_t>(entries[categoryIdx].first));
		}

		// ---------------------------------------------------------
		// 2. Mask の設定 (CheckboxFlags)
		// ---------------------------------------------------------
		ImGui::Spacing();
		ImGui::Text("Collides With (Mask):");

		uint32_t currentMask = _c->GetMaskBits();

		if(ImGui::Button("Select All")) {
			currentMask = 0xFFFFFFFF; // ALL定数
			_c->SetMaskBits(currentMask);
		}
		ImGui::SameLine();
		if(ImGui::Button("Clear All")) {
			currentMask = 0;
			_c->SetMaskBits(currentMask);
		}

		// 各レイヤーをチェックボックスとして表示 (複数選択可能)
		for(const auto& entry : entries) {
			uint32_t bitValue = static_cast<uint32_t>(entry.first);
			if(ImGui::CheckboxFlags(entry.second.data(), &currentMask, bitValue)) {
				_c->SetMaskBits(currentMask);
			}
		}
	}



	/// sphere parameter
	ImGui::SeparatorText("sphere parameter");
	Editor::ImMathf::DragFloat("radius", &_c->radius_, 0.1f, 0.0f, 100.0f);

}


/**
 * @brief JSONからのデシリアライズ
 */
void ONEngine::from_json(const nlohmann::json& _j, SphereCollider& _s) {
	_s.enable = _j.value("enable", 1);
	_s.radius_ = _j.value("radius", 1.0f);
	_s.isTrigger_ = _j.value("isTrigger", false);
	_s.freezeY_ = _j.value("freezeY", false);
	_s.mass_ = _j.value("mass", 1.0f);
	_s.collisionState_ = magic_enum::enum_cast<CollisionState>(_j.value("state", "Dynamic")).value_or(CollisionState::Dynamic);
	_s.categoryBits_ = _j.value("categoryBits", static_cast<uint32_t>(CollisionFilter::Default));
	_s.maskBits_ = _j.value("maskBits", static_cast<uint32_t>(CollisionFilter::ALL));
}

/**
 * @brief JSONへのシリアライズ
 */
void ONEngine::to_json(nlohmann::json& _j, const SphereCollider& _s) {
	_j = nlohmann::json{
		{ "type", "SphereCollider" },
		{ "enable", _s.enable },
		{ "radius", _s.GetRadius() },
		{ "isTrigger", _s.IsTrigger() },
		{ "freezeY", _s.freezeY_ },
		{ "mass", _s.mass_ },
		{ "state", magic_enum::enum_name(_s.collisionState_) },
		{ "categoryBits", _s.categoryBits_ },
		{ "maskBits", _s.maskBits_ }
	};
}


/**
 * @brief コンストラクタ
 */
SphereCollider::SphereCollider() {
	// デフォルトの値をセット
	radius_ = 1.0f;
}

/**
 * @brief 球体コライダーの半径を設定します。
 */
void SphereCollider::SetRadius(float _radius) {
	radius_ = _radius;
}

/**
 * @brief 球体コライダーの半径を取得します。
 */
float SphereCollider::GetRadius() const {
	return radius_;
}

/**
 * @brief C#（Mono）インターフェース用：コライダーの半径を取得
 */
float ONEngine::InternalGetRadius(uint64_t _nativeHandle) {
	SphereCollider* c = reinterpret_cast<SphereCollider*>(_nativeHandle);
	return c ? c->GetRadius() : 0.0f;
}

/**
 * @brief C#（Mono）インターフェース用：コライダーの半径を設定
 */
void ONEngine::InternalSetRadius(uint64_t _nativeHandle, float _radius) {
	SphereCollider* c = reinterpret_cast<SphereCollider*>(_nativeHandle);
	if(c) c->SetRadius(_radius);
}

/**
 * @brief C#（Mono）インターフェース用：トリガーモードかの判定を取得
 */
bool ONEngine::InternalIsTriggerSphere(uint64_t _nativeHandle) {
	SphereCollider* c = reinterpret_cast<SphereCollider*>(_nativeHandle);
	return c ? c->IsTrigger() : false;
}

/**
 * @brief C#（Mono）インターフェース用：トリガーモードの設定を変更
 */
void ONEngine::InternalSetTriggerSphere(uint64_t _nativeHandle, bool _trigger) {
	SphereCollider* c = reinterpret_cast<SphereCollider*>(_nativeHandle);
	if(c) c->SetTrigger(_trigger);
}

/**
 * @brief C#（Mono）インターフェース用：質量を取得
 */
float ONEngine::InternalGetMass(uint64_t _nativeHandle) {
	SphereCollider* c = reinterpret_cast<SphereCollider*>(_nativeHandle);
	return c ? c->GetMass() : 1.0f;
}

/**
 * @brief C#（Mono）インターフェース用：質量を設定
 */
void ONEngine::InternalSetMass(uint64_t _nativeHandle, float _mass) {
	SphereCollider* c = reinterpret_cast<SphereCollider*>(_nativeHandle);
	if(c) c->SetMass(_mass);
}

