#include "SphereCollider.h"

#include <magic_enum/magic_enum.hpp>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"


void COMP_DEBUG::SphereColliderDebug(SphereCollider* _c) {
	if (!_c) {
		return;
	}

	/// 共通パラメータ
	ImGui::SeparatorText("common parameter");
	int currentIndex = static_cast<int>(_c->collisionState_);

	auto names = magic_enum::enum_names<CollisionState>();
	std::vector<const char*> items;
	for (auto& n : names) items.push_back(n.data());

	if (ImGui::Combo("CollisionState", &currentIndex, items.data(), (int)items.size())) {
		_c->collisionState_ = static_cast<CollisionState>(currentIndex);
	}



	/// sphere parameter
	ImGui::SeparatorText("sphere parameter");
	ImMathf::DragFloat("radius", &_c->radius_, 0.1f, 0.0f, 100.0f);

}


void from_json(const nlohmann::json& _j, SphereCollider& _s) {
	_s.enable = _j.value("enable", 1);
	_s.radius_ = _j.value("radius", 1.0f);
	_s.collisionState_ = magic_enum::enum_cast<CollisionState>(_j.value("state", "Dynamic")).value_or(CollisionState::Dynamic);
}

void to_json(nlohmann::json& _j, const SphereCollider& _s) {
	_j = nlohmann::json{
		{ "type", "SphereCollider" },
		{ "enable", _s.enable },
		{ "radius", _s.GetRadius() },
		{ "state", magic_enum::enum_name(_s.collisionState_) }
	};
}


SphereCollider::SphereCollider() {
	// デフォルトの値をセット
	radius_ = 1.0f;
}

void SphereCollider::SetRadius(float _radius) {
	radius_ = _radius;
}

float SphereCollider::GetRadius() const {
	return radius_;
}

