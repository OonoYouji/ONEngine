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
namespace ONEngine {

class ScreenPostEffectTag : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ScreenPostEffectTag() = default;
	~ScreenPostEffectTag() override = default;

	/// @brief ポストエフェクトの有効/無効を設定する
	/// @param _type 対象のポストエフェクトの種類
	/// @param _enable true: 有効 false: 無効
	void SetPostEffectEnable(PostEffectType _type, bool _enable);

	/// @brief 指定した種類のポストエフェクトが有効かどうかを返す
	/// @param _type 確認するポストエフェクトの種類
	/// @return true: 有効 false: 無効
	bool GetPostEffectEnable(PostEffectType _type) const;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

};


namespace COMP_DEBUG {
	void ScreenPostEffectTagDebug(ScreenPostEffectTag* _component);
}


/// Json変換
void from_json(const nlohmann::json& _j, ScreenPostEffectTag& _c);
void to_json(nlohmann::json& _j, const ScreenPostEffectTag& _c);

} /// ONEngine
