#include "ScreenPostEffectTag.h"

/// std
#include <vector>

/// external
#include <imgui.h>

/// std
#include <string>

namespace {

	struct Flags {
		Flags() : flags(PostEffectType_Count, false) {}
		std::vector<bool> flags;
		std::vector<std::string> flagNames = {
			"Grayscale",
			"Radial Blur" 
		};
	};

	Flags gFlags;

} /// namespace

void ScreenPostEffectTag::SetPostEffectEnable(PostEffectType _type, bool _enable) {
	gFlags.flags[static_cast<size_t>(_type)] = _enable;
}

bool ScreenPostEffectTag::GetPostEffectEnable(PostEffectType _type) const {
	return gFlags.flags[static_cast<size_t>(_type)];
}



void COMP_DEBUG::ScreenPostEffectTagDebug(ScreenPostEffectTag* _component) {
	if (!_component) {
		return;
	}

	for (size_t i = 0; i < gFlags.flags.size(); ++i) {
		bool flag = gFlags.flags[i];

		ImGui::Checkbox(gFlags.flagNames[i].c_str(), &flag);

		gFlags.flags[i] = flag;
	}

}

void from_json(const nlohmann::json& _j, ScreenPostEffectTag& _c) {
	if (_j.contains("enable")) {
		_c.enable = _j["enable"].get<int>();
	}
	if (_j.contains("id")) {
		_c.id = _j["id"].get<uint32_t>();
	}
	// Handle post effect flags if they are present in the JSON
	if (_j.contains("postEffects")) {
		for (const auto& effect : _j["postEffects"]) {
			auto type = effect["type"].get<int>();
			bool enabled = effect["enabled"].get<bool>();
			_c.SetPostEffectEnable(PostEffectType(type), enabled);
		}
	}
}

void to_json(nlohmann::json& _j, const ScreenPostEffectTag& _c) {
	_j["type"] = "ScreenPostEffectTag";
	_j["enable"] = _c.enable;
	_j["id"] = _c.id;
	// Serialize post effect flags
	for (size_t i = 0; i < gFlags.flags.size(); ++i) {
		_j["postEffects"].push_back({
			{ "type", i },
			{ "enabled", gFlags.flags[i] }
			});
	}

}
