#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"


/// @brief ComponentDebug名前空間ように前方宣言
namespace ONEngine {

class ShadowCaster;

namespace ComponentDebug {
void ShadowCasterDebug(ShadowCaster* _shadowCaster);
}

void from_json(const nlohmann::json& _j, ShadowCaster& _c);
void to_json(nlohmann::json& _j, const ShadowCaster& _c);

/// @brief 影に関するパラメータ構造体
struct ShadowParameter {
	Vector2 screenSize;
	Vector2 texelSizeShadow;
	float shadowBias;
	float shadowDarkness;
	int pcfRadius;
};


/// ///////////////////////////////////////////////////
/// 影の投影を行うためのコンポーネント
/// ///////////////////////////////////////////////////
/**
 * @class ShadowCaster
 * @brief ゲーム世界内で影を生成・描画するためのライトビュー射影行列やシャドウマップテクスチャ用のカメラ、およびシャドウパラメータを管理するコンポーネントクラス
 */
class ShadowCaster : public IComponent {
	friend void ComponentDebug::ShadowCasterDebug(ShadowCaster* _shadowCaster);
	friend void from_json(const nlohmann::json& _j, ShadowCaster& _c);
	friend void to_json(nlohmann::json& _j, const ShadowCaster& _c);
public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	/**
	 * @brief コンストラクタ
	 */
	ShadowCaster();

	/**
	 * @brief デストラクタ
	 */
	~ShadowCaster() override;


	/**
	 * @brief 内部のシャドウマッピング用深度記述カメラなどを生成し、シャドウキャスターとしての初期設定を行います。
	 */
	void CreateShadowCaster();

	/**
	 * @brief 指定されたディレクショナルライトの照射方向に基づいて、影用のビュー・プロジェクション行列を再計算して更新します。
	 * @param _ecsGroup ECS管理グループ（他カメラやオーナー検索用）
	 * @param _directionLight 影を生成する元となる平行光源
	 */
	void CalculationLightViewMatrix(class ECSGroup* _ecsGroup, class DirectionalLight* _directionLight);


	/**
	 * @brief 影描画のレンダリングに使用するカメラコンポーネントのポインタを取得します。
	 */
	CameraComponent* GetShadowCasterCamera();

	/**
	 * @brief PCF半径やシャドウバイアスなど、シェーダに渡すための影パラメータ構造体を取得します。
	 */
	ShadowParameter GetShadowParameters() const;

private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	bool isCreated_;

	CameraComponent* camera_;


	Vector3 baseLightPos_;
	float lightLength_;

	Vector2 orthographicSize_;
	float scaleFactor_;
	Vector2 texelSizeShadow_;
	float shadowBias_;
	float shadowDarkness_;
	int pcfRadius_;

};


} /// ONEngine
