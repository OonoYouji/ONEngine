#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"

#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"


/// @brief COMP_DEBUG名前空間ように前方宣言
class ShadowCaster;

namespace COMP_DEBUG {
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
class ShadowCaster : public IComponent {
	friend void COMP_DEBUG::ShadowCasterDebug(ShadowCaster* _shadowCaster);
	friend void from_json(const nlohmann::json& _j, ShadowCaster& _c);
	friend void to_json(nlohmann::json& _j, const ShadowCaster& _c);
public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	ShadowCaster();
	~ShadowCaster() override;


	/// @brief 自身をシャドウキャスターとして作成
	void CreateShadowCaster();

	/// @brief DirectionalLightからライトビュー行列を設定
	/// @param _directionLight DirectionalLightへのポインタ
	void CalculationLightViewMatrix(class ECSGroup* _ecsGroup, class DirectionalLight* _directionLight);


	/// @brief 影の投影用のカメラを取得する
	/// @return 投影用カメラ
	CameraComponent* GetShadowCasterCamera();

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

