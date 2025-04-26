#include "Gizmo.h"

/// std
#include <memory>

namespace {

	/// ///////////////////////////////////////////////////
	/// gizmoの描画データを保持するクラス
	/// ///////////////////////////////////////////////////
	class GizmoSystem {
		friend class Gizmo;
	public:

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

	public:
		/*##########################################################
			TODO : COMMENT
			仮に１つの値ですべてのreserveを行うがこれを別々の引数にする
		##########################################################*/
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
		std::vector<SphereData> sphereData_;
		std::vector<CubeData>   cubeData_;

		/// wire data
		std::vector<SphereData> wireSphereData_;
		std::vector<CubeData>   wireCubeData_;
		std::vector<LineData>   lineData_;

	};


	/// 宣言
	std::unique_ptr<GizmoSystem> gGizmoSystem = nullptr;

} /// namespace 



void Gizmo::Initialize(const size_t _maxDrawInstanceCount) {
	gGizmoSystem = std::make_unique<GizmoSystem>(_maxDrawInstanceCount);
}

void Gizmo::DrawSphere(const Vector3& _position, float _radius, const Vector4& _color) {
	gGizmoSystem->sphereData_.push_back({ _position, _radius, _color });
}

void Gizmo::DrawWireSphere(const Vector3& _position, float _radius, const Vector4& _color) {
	gGizmoSystem->wireSphereData_.push_back({ _position, _radius, _color });
}

void Gizmo::DrawCube(const Vector3& _position, const Vector3& _size, const Vector4& _color) {
	gGizmoSystem->cubeData_.push_back({ _position, _size, _color });
}

void Gizmo::DrawWireCube(const Vector3& _position, const Vector3& _size, const Vector4& _color) {
	gGizmoSystem->wireCubeData_.push_back({ _position, _size, _color });
}

void Gizmo::DrawLine(const Vector3& _startPosition, const Vector3& _endPosition, const Vector4& _color) {
	gGizmoSystem->lineData_.push_back({ _startPosition, _endPosition, _color });
}

void Gizmo::DrawRay(const Vector3& _position, const Vector3& _direction, const Vector4& _color) {
	gGizmoSystem->lineData_.push_back({ _position, _position + _direction, _color });
}
