#pragma once

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Buffer/ConstantBuffer.h"
#include "Engine/Graphics/Buffer/Data/ViewProjection.h"
#include <Engine/Core/Utility/Math/Vector3.h>


/// ----- 前方宣言 ----- ///
namespace ONEngine {

class CameraComponent;

/// @brief カメラの種類
enum class CameraType {
	Type3D, ///< 3Dカメラ
	Type2D, ///< 2Dカメラ
};


/// @brief Componentのデバッグ関数
namespace ComponentDebug {
void CameraDebug(CameraComponent* _camera);
}

namespace ComponentApplyFuncs {
void ApplyCamera(void* _element, class ECSGroup* _ecsGroup);
void FetchCamera(void* _element, class ECSGroup* _ecsGroup);
}

/// Json変換
void from_json(const nlohmann::json& _j, CameraComponent& _c);
void to_json(nlohmann::json& _j, const CameraComponent& _c);


/// ///////////////////////////////////////////////////
/// カメラのコンポーネント
/// ///////////////////////////////////////////////////
/**
 * @class CameraComponent
 * @brief 3Dおよび2Dの描画範囲を制御するための視野、射影設定、ビュー・プロジェクション行列（ViewProjection）定数バッファを管理するカメラコンポーネントクラス
 */
class CameraComponent : public IComponent {
	/// ----- friend class ----- ///
	friend class CameraUpdateSystem;

	/// ----- friend function ----- ///
	friend void ComponentDebug::CameraDebug(CameraComponent* _camera);
	friend void ComponentApplyFuncs::ApplyCamera(void* _element, class ECSGroup* _ecsGroup);
	friend void ComponentApplyFuncs::FetchCamera(void* _element, class ECSGroup* _ecsGroup);
	friend void from_json(const nlohmann::json& _j, CameraComponent& _c);
	friend void to_json(nlohmann::json& _j, const CameraComponent& _c);

public:

	struct FogParams {
		Vector3 color;   // フォグの色
		float fogStart; // フォグの開始距離
		float fogEnd;   // フォグの終了距離
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 */
	CameraComponent();

	/**
	 * @brief デストラクタ
	 */
	~CameraComponent() override;

	/**
	 * @brief トランスフォームと設定値（FOV等）からビュー行列、プロジェクション行列を再計算して更新します。
	 */
	void UpdateViewProjection();


	/**
	 * @brief 指定されたワールド空間のAABB（中心とサイズ）がカメラの視錐台（フラスタム）の範囲内に交差しているかを判定します。
	 * @param center 判定対象AABBの中心点
	 * @param size 判定対象AABBの3軸サイズ
	 * @return 視錐台の内部または交差している場合は true、完全に外側の場合は false
	 */
	bool IsVisible(const Vector3& center, const Vector3& size) const;

	/**
	 * @brief 指定された目標の方向（あるいは注視点ベクトル）を向くようにカメラの回転トランスフォームを更新します。
	 * @param direction 目標方向
	 */
	void LookAt(const Vector3& direction);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/**
	 * @brief ビュー・プロジェクション行列をGPUへ転送するための定数バッファを生成します。
	 * @param _dxDevice 生成に使用するDxDevice
	 */
	void MakeViewProjection(class DxDevice* _dxDevice);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ConstantBuffer<ViewProjection> viewProjection_;
	ConstantBuffer<Vector4> cameraPosBuffer_;
	ConstantBuffer<FogParams> cBufferFogParams_;

	float fovY_;
	float nearClip_;
	float farClip_;

	Matrix4x4 matView_;
	Matrix4x4 matProjection_;

	int cameraType_;
	bool isMainCameraRequest_;
	bool isDrawFrustum_;

	Vector2 orthographicSize_;

	/// fog parameters
	FogParams fogParams_;

public:
	/// ====================================================
	/// public : accessor
	/// ====================================================

	/**
	 * @brief このカメラをメインカメラとして登録要求するかどうかを設定します。
	 */
	void SetIsMainCameraRequest(bool _isMainCamera);

	/**
	 * @brief カメラの投影タイプ（3D/2D）を設定します。
	 * @param _cameraType CameraType（Type3D=0, Type2D=1等）
	 */
	void SetCameraType(int _cameraType);

	/**
	 * @brief 2D平行投影時の縦横サイズを設定します。
	 */
	void SetOrthographicSize(const Vector2& _size);


	/**
	 * @brief メインカメラ要求フラグの状態を取得します。
	 */
	bool GetIsMainCameraRequest() const;

	/**
	 * @brief 現在のカメラタイプ（3D/2D）を取得します。
	 */
	int GetCameraType() const;

	/**
	 * @brief 定数バッファがすでに正しく生成されているかを判定します。
	 */
	bool IsMakeViewProjection() const;

	/**
	 * @brief 算出されたビュー・プロジェクション行列（CPUデータ）の参照を取得します。
	 */
	const ViewProjection& GetViewProjection() const;

	/**
	 * @brief GPUへバインドするためのビュー・プロジェクション定数バッファ参照を取得します。
	 */
	ConstantBuffer<ViewProjection>& GetViewProjectionBuffer();

	/**
	 * @brief カメラのワールド位置座標を送るための定数バッファ参照を取得します。
	 */
	ConstantBuffer<Vector4>& GetCameraPosBuffer();

	/**
	 * @brief フォグ設定パラメータを送るための定数バッファ参照を取得します。
	 */
	ConstantBuffer<FogParams>& GetFogParamsBuffer();

	/**
	 * @brief ビュー行列（matView）を取得します。
	 */
	const Matrix4x4& GetViewMatrix() const;

	/**
	 * @brief 射影（プロジェクション）行列を取得します。
	 */
	const Matrix4x4& GetProjectionMatrix() const;

};



/// @brief カメラ関連の数学関数群
namespace CameraMath {

/**
 * @brief 視野角（Y軸）、アスペクト比、クリッピング面から透視投影（パースペクティブ）行列を作成します。
 * @param _fovY 垂直視野角（ラジアン）
 * @param _aspectRatio アスペクト比
 * @param _nearClip 手前クリップ面距離
 * @param _farClip 奥クリップ面距離
 * @return 射影行列
 */
Matrix4x4 MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip);

/**
 * @brief 平行投影（オーソグラフィック）行列を作成します。
 * @param _left 描画範囲の左端
 * @param _right 描画範囲の右端
 * @param _bottom 描画範囲の下端
 * @param _top 描画範囲の上端
 * @param _znear 手前クリップ面距離
 * @param _zfar 奥クリップ面距離
 * @return 射影行列
 */
Matrix4x4 MakeOrthographicMatrix(float _left, float _right, float _bottom, float _top, float _znear, float _zfar);

}


} /// ONEngine
