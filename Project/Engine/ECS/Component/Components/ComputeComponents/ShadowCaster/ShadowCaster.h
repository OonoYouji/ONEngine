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


/// ///////////////////////////////////////////////////
/// 影の投影を行うためのコンポーネント
/// ///////////////////////////////////////////////////
class ShadowCaster : public IComponent {
public:
	/// ===========================================
	/// public : methods
	/// ===========================================

	ShadowCaster();
	~ShadowCaster() override;


	/// @brief 自身をシャドウキャスターとして作成
	/// @param _dxDevice DxDeviceへのポインタ
	void CreateShadowCaster(class DxDevice* _dxDevice);

	/// @brief ViewProjectionバッファの作成
	/// @param _dxDevice DxDeviceへのポインタ
	void CreateVPBuffer(DxDevice* _dxDevice);

	/// @brief DirectionalLightからライトビュー行列を設定
	/// @param _directionLight DirectionalLightへのポインタ
	void CalculationLightViewMatrix(class DirectionalLight* _directionLight);


	/// @brief 影の投影用のカメラを取得する
	/// @return 投影用カメラ
	CameraComponent* GetShadowCasterCamera();

private:
	/// ===========================================
	/// private : objects
	/// ===========================================

	CameraComponent* camera_;
};

