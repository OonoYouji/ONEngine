#pragma once

#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Color.h"
#include "Engine/Core/Utility/Math/Quaternion.h"

/// ///////////////////////////////////////////////////
/// gizmoのクラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

class Gizmo final {
	friend class GizmoRenderingPipeline;
	friend class RenderingFramework;
public:
	/// ====================================
	/// private : sub class 
	/// ====================================

	struct SphereData {
		Vector3 position; ///< 球の位置
		float radius;     ///< 球の半径
		Vector4 color;    ///< 球の色
	};

	struct CubeData {
		Vector3 position;    ///< 箱の位置
		Vector3 size;        ///< 箱のサイズ
		Quaternion rotate;   ///< 箱の回転
		Vector4 color;       ///< 箱の色
	};

	struct LineData {
		Vector3 startPosition; ///< 線の開始地点
		Vector3 endPosition;   ///< 線の終了地点
		Vector4 color;         ///< 線の色
		float thickness;       ///< 線の太さ
	};

private:
	/// ====================================
	/// private : methods
	/// ====================================

	/**
	 * @brief プライベートコンストラクタ（静的ユーティリティクラス）
	 */
	Gizmo() = default;

	/**
	 * @brief デストラクタ
	 */
	~Gizmo() = default;

	/**
	 * @brief ギズモシステムの初期化を行います（描画インスタンス上限の設定など）。
	 * @param _maxDrawInstanceCount 最大描画インスタンス数
	 */
	static void Initialize(const size_t _maxDrawInstanceCount);

	/// ----- 各形状の配列を返す ----- ///
	static const std::vector<SphereData>& GetSphereData();
	static const std::vector<SphereData>& GetWireSphereData();
	static const std::vector<CubeData>& GetCubeData();
	static const std::vector<CubeData>& GetWireCubeData();
	static const std::vector<LineData>& GetLineData();

	/**
	 * @brief 蓄積された描画要求ギズモデータをクリア（リセット）します。毎フレームの開始時に呼び出されます。
	 */
	static void Reset();

public:
	/// ====================================
	/// public : static methods
	/// ====================================

	/**
	 * @brief デバッグ用のソリッド球体を描画登録します。
	 * @param _position 球の中心（ワールド座標）
	 * @param _radius 球の半径
	 * @param _color 描画色（RGBA）
	 */
	static void DrawSphere(const Vector3& _position, float _radius, const Vector4& _color = Color::kWhite);

	/**
	 * @brief デバッグ用のワイヤーフレーム球体を描画登録します。
	 * @param _position 球の中心（ワールド座標）
	 * @param _radius 球の半径
	 * @param _color 描画色（RGBA）
	 */
	static void DrawWireSphere(const Vector3& _position, float _radius, const Vector4& _color = Color::kWhite);

	/**
	 * @brief デバッグ用のソリッド立方体（OBB対応）を描画登録します。
	 * @param _position 立方体の中心座標
	 * @param _size 立方体の各軸方向のサイズ
	 * @param _rotate 立方体の回転（クォータニオン）
	 * @param _color 描画色
	 */
	static void DrawCube(const Vector3& _position, const Vector3& _size, const Quaternion& _rotate = Quaternion::kIdentity, const Vector4& _color = Color::kWhite);

	/**
	 * @brief デバッグ用のワイヤーフレーム立方体（OBB対応）を描画登録します。
	 * @param _position 立方体の中心座標
	 * @param _size 立方体の各軸方向のサイズ
	 * @param _rotate 立方体の回転
	 * @param _color 描画色
	 */
	static void DrawWireCube(const Vector3& _position, const Vector3& _size, const Quaternion& _rotate = Quaternion::kIdentity, const Vector4& _color = Color::kWhite);

	/**
	 * @brief デバッグ用の線分を描画登録します。
	 * @param _startPosition 線の始点座標
	 * @param _endPosition 線の終点座標
	 * @param _color 描画色
	 * @param _thickness 線の太さ（ピクセル幅）
	 */
	static void DrawLine(const Vector3& _startPosition, const Vector3& _endPosition, const Vector4& _color = Color::kWhite, float _thickness = 1.0f);

	/**
	 * @brief デバッグ用の光線（始点と方向ベクトル）を描画登録します。
	 * @param _position 光線の始点
	 * @param _direction 光線の方向ベクトル
	 * @param _color 描画色
	 * @param _thickness 線の太さ
	 */
	static void DrawRay(const Vector3& _position, const Vector3& _direction, const Vector4& _color = Color::kWhite, float _thickness = 1.0f);

};


} /// ONEngine
