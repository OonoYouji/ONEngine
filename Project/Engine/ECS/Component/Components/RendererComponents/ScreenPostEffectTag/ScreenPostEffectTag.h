#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"

/// @brief ポストエフェクトの種類
enum PostEffectType {
	PostEffectType_Grayscale,  ///< グレースケール
	PostEffectType_RadialBlur, ///< ラジアルブラー
	PostEffectType_Count	   ///< 要素数
};

/// ///////////////////////////////////////////////////
/// スクリーンにかけるポストエフェクトのフラグを持つコンポーネント
/// ///////////////////////////////////////////////////
class ScreenPostEffectTag : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ScreenPostEffectTag() = default;
	~ScreenPostEffectTag() override = default;

	void SetPostEffectEnable(PostEffectType _type, bool _enable);
	bool GetPostEffectEnable(PostEffectType _type) const;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

};


namespace COMP_DEBUG {
	void ScreenPostEffectTagDebug(ScreenPostEffectTag* _component);
}

void from_json(const nlohmann::json& _j, ScreenPostEffectTag& _c);
void to_json(nlohmann::json& _j, const ScreenPostEffectTag& _c);