#include "Gizmo.h"

using namespace ONEngine;

/// std
#include <memory>

/// engine
#include "Engine/Core/Config/EngineConfig.h"

namespace {

	/// ///////////////////////////////////////////////////
	/// gizmoの描画データを保持するクラス
	/// ///////////////////////////////////////////////////
	class GizmoSystem {
		friend class Gizmo;
	public:
		GizmoSystem(const size_t _maxDrawInstanceCount)
			: maxInstanceCount_(_maxDrawInstanceCount) {

			/// 描画データのメモリを確保しておく
			sphereData_.reserve(maxInstanceCount_);
			cubeData_.reserve(maxInstanceCount_);

			wireSphereData_.reserve(maxInstanceCount_);
			wireCubeData_.reserve(maxInstanceCount_);
			lineData_.reserve(maxInstanceCount_);
		}
		~GizmoSystem() = default;

	private:
		const size_t maxInstanceCount_;

		/// solid data
		std::vector<Gizmo::SphereData> sphereData_;
		std::vector<Gizmo::CubeData>   cubeData_;

		/// wire data
		std::vector<Gizmo::SphereData> wireSphereData_;
		std::vector<Gizmo::CubeData>   wireCubeData_;
		std::vector<Gizmo::LineData>   lineData_;

	};


	/// 宣言
	std::unique_ptr<GizmoSystem> gGizmoSystem = nullptr;

} /// namespace 



/**
 * @brief ギズモシステムの初期化を行います。
 * @param _maxDrawInstanceCount 最大描画インスタンス数
 */
void Gizmo::Initialize(const size_t _maxDrawInstanceCount) {
	gGizmoSystem = std::make_unique<GizmoSystem>(_maxDrawInstanceCount);
}



const std::vector<Gizmo::SphereData>& Gizmo::GetSphereData() {
	return gGizmoSystem->sphereData_;
}

const std::vector<Gizmo::SphereData>& Gizmo::GetWireSphereData() {
	return gGizmoSystem->wireSphereData_;
}

const std::vector<Gizmo::CubeData>& Gizmo::GetCubeData() {
	return gGizmoSystem->cubeData_;
}

const std::vector<Gizmo::CubeData>& Gizmo::GetWireCubeData() {
	return gGizmoSystem->wireCubeData_;
}

const std::vector<Gizmo::LineData>& Gizmo::GetLineData() {
	return gGizmoSystem->lineData_;
}

/**
 * @brief 蓄積された描画要求ギズモデータをクリアします。毎フレームの開始時に呼び出されます。
 */
void Gizmo::Reset() {
	gGizmoSystem->sphereData_.clear();
	gGizmoSystem->cubeData_.clear();
	gGizmoSystem->wireSphereData_.clear();
	gGizmoSystem->wireCubeData_.clear();
	gGizmoSystem->lineData_.clear();
}


#ifdef DEBUG_MODE

/**
 * @brief デバッグ用のソリッド球体を描画登録します。
 */
void Gizmo::DrawSphere(const Vector3& _position, float _radius, const Vector4& _color) {
	gGizmoSystem->sphereData_.push_back({ _position, _radius, _color });
}

/**
 * @brief デバッグ用のワイヤーフレーム球体を描画登録します。
 */
void Gizmo::DrawWireSphere(const Vector3& _position, float _radius, const Vector4& _color) {
	gGizmoSystem->wireSphereData_.push_back({ _position, _radius, _color });
}

/**
 * @brief デバッグ用のソリッド立方体（OBB対応）を描画登録します。
 */
void Gizmo::DrawCube(const Vector3& _position, const Vector3& _size, const Quaternion& _rotate, const Vector4& _color) {
	gGizmoSystem->cubeData_.push_back({ _position, _size, _rotate, _color });
}

/**
 * @brief デバッグ用のワイヤーフレーム立方体（OBB対応）を描画登録します。
 */
void Gizmo::DrawWireCube(const Vector3& _position, const Vector3& _size, const Quaternion& _rotate, const Vector4& _color) {
	gGizmoSystem->wireCubeData_.push_back({ _position, _size, _rotate, _color });
}

/**
 * @brief デバッグ用の線分を描画登録します。
 */
void Gizmo::DrawLine(const Vector3& _startPosition, const Vector3& _endPosition, const Vector4& _color, float _thickness) {
	if (!gGizmoSystem) return;
	gGizmoSystem->lineData_.push_back({ _startPosition, _endPosition, _color, _thickness });
}

/**
 * @brief デバッグ用の光線を描画登録します。
 */
void Gizmo::DrawRay(const Vector3& _position, const Vector3& _direction, const Vector4& _color, float _thickness) {
	if (!gGizmoSystem) return;
	gGizmoSystem->lineData_.push_back({ _position, _position + _direction, _color, _thickness });
}

#else /// RELEASE_BUILD
/// リリース用に空の関数を定義
void Gizmo::DrawSphere(const Vector3&, float, const Vector4&) {}
void Gizmo::DrawWireSphere(const Vector3&, float, const Vector4&) {}
void Gizmo::DrawCube(const Vector3&, const Vector3&, const Quaternion&, const Vector4&) {}
void Gizmo::DrawWireCube(const Vector3&, const Vector3&, const Quaternion&, const Vector4&) {}
void Gizmo::DrawLine(const Vector3&, const Vector3&, const Vector4&, float) {}
void Gizmo::DrawRay(const Vector3&, const Vector3&, const Vector4&, float) {}
#endif // DEBUG_BUILD
