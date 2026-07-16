#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"

/**
 * @enum PostEffectType
 * @brief 適用するポストプロセス（画面効果）のタイプ列挙
 */
enum PostEffectType {
	PostEffectType_Grayscale,  ///< グレースケール（白黒）
	PostEffectType_RadialBlur, ///< ラジアルブラー（放射状ブラー）
	PostEffectType_Count	   ///< ポストエフェクトの種類数（ガード用）
};

/// ///////////////////////////////////////////////////
/// スクリーンにかけるポストエフェクトのフラグを持つコンポーネント
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ScreenPostEffectTag
 * @brief レンダーターゲットに対して適用する各種ポストエフェクト（グレースケールやブラーなど）の有効状態タグフラグを保持するコンポーネントクラス
 */
class ScreenPostEffectTag : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	ScreenPostEffectTag() = default;

	/**
	 * @brief デストラクタ
	 */
	~ScreenPostEffectTag() override = default;

	/**
	 * @brief 指定したポストプロセスの有効/無効を設定します。
	 * @param _type 対象のポストプロセスの種類
	 * @param _enable 有効にする場合は true、無効にする場合は false
	 */
	void SetPostEffectEnable(PostEffectType _type, bool _enable);

	/**
	 * @brief 指定したポストプロセスが有効であるかを判定します。
	 * @param _type 対象のポストプロセスの種類
	 * @return 有効である場合は true、無効である場合は false
	 */
	bool GetPostEffectEnable(PostEffectType _type) const;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

};


namespace ComponentDebug {
    /**
     * @brief エディタ用：ScreenPostEffectTagコンポーネントのデバッグ表示（Gui描画等）処理を行います。
     */
	void ScreenPostEffectTagDebug(ScreenPostEffectTag* _component);
}


/**
 * @brief JSONからのデシリアライズ
 */
void from_json(const nlohmann::json& _j, ScreenPostEffectTag& _c);

/**
 * @brief JSONへのシリアライズ
 */
void to_json(nlohmann::json& _j, const ScreenPostEffectTag& _c);

} /// ONEngine
