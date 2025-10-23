#pragma once

#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Color.h"

/// ///////////////////////////////////////////////////
/// gizmoのクラス
/// ///////////////////////////////////////////////////
class Gizmo final {
	friend class GizmoRenderingPipeline;
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
		Vector3 position; ///< 箱の位置
		Vector3 size;     ///< 箱のサイズ
		Vector4 color;    ///< 箱の色
	};

	struct LineData {
		Vector3 startPosition; ///< 線の開始地点
		Vector3 endPosition;   ///< 線の終了地点
		Vector4 color;         ///< 線の色
	};

private:
	/// ====================================
	/// private : methods
	/// ====================================

	Gizmo() = default;
	~Gizmo() = default;

	/// @brief 初期化関数、rendering pipelineクラスで呼び出す
	/// @param _maxDrawInstanceCount 描画するインスタンスの最大数を返す
	static void Initialize(const size_t _maxDrawInstanceCount);

	/// ----- 各形状の配列を返す ----- ///
	static const std::vector<SphereData>& GetSphereData();
	static const std::vector<SphereData>& GetWireSphereData();
	static const std::vector<CubeData>& GetCubeData();
	static const std::vector<CubeData>& GetWireCubeData();
	static const std::vector<LineData>& GetLineData();

	/// @brief データのリセット
	static void Reset();

public:
	/// ====================================
	/// public : static methods
	/// ====================================

	/// @brief 球の描画
	/// @param _position ワールド座標
	/// @param _radius 球の半径
	/// @param _color 球の色
	static void DrawSphere(const Vector3& _position, float _radius, const Vector4& _color = Color::kWhite);

	/// @brief ワイヤーフレームの球を描画
	/// @param _position ワールド座標
	/// @param _radius 球の半径
	/// @param _color 球の色
	static void DrawWireSphere(const Vector3& _position, float _radius, const Vector4& _color = Color::kWhite);

	/// @brief 箱の描画
	/// @param _position ワールド座標
	/// @param _size 箱のサイズ
	/// @param _color 箱の色
	static void DrawCube(const Vector3& _position, const Vector3& _size, const Vector4& _color = Color::kWhite);

	/// @brief ワイヤーフレームの箱を描画
	/// @param _position ワールド座標
	/// @param _size 箱のサイズ
	/// @param _color 箱の色 
	static void DrawWireCube(const Vector3& _position, const Vector3& _size, const Vector4& _color = Color::kWhite);

	/// @brief 線の描画
	/// @param _startPosition 線の始点
	/// @param _endPosition 線の終点
	/// @param _color 線の色
	static void DrawLine(const Vector3& _startPosition, const Vector3& _endPosition, const Vector4& _color = Color::kWhite);

	/// @brief Rayの描画
	/// @param _position 線の始点 
	/// @param _direction 線の方向
	/// @param _color 線の色
	static void DrawRay(const Vector3& _position, const Vector3& _direction, const Vector4& _color = Color::kWhite);

};

