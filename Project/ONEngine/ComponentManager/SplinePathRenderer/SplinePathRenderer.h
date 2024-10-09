#pragma once

#include <cmath>
#include <list>
#include <array>

#include <Math/Vector3.h>

#include <ComponentManager/Base/BaseComponent.h>
#include <vector>


/// ===================================================
/// スプライン曲線を描画するコンポーネント
/// ===================================================
class SplinePathRenderer final : public BaseComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SplinePathRenderer(uint32_t segmentDivisions);
	~SplinePathRenderer() {}

	void Initialize() override;
	void Update()     override;
	void Draw()       override;


	/// <summary>
	/// アンカーポイントの追加
	/// </summary>
	/// <param name="anchorPoint">スプライン曲線の制御点</param>
	void AddAnchorPoint(const Vec3& anchorPoint);

	/// <summary>
	/// アンカーポイントの配列をセットする
	/// </summary>
	/// <param name="anchorPointArray"></param>
	void SetAnchorPointArray(const std::vector<Vec3>& anchorPointArray);

	/// <summary>
	/// スプライン曲線の点を計算する
	/// </summary>
	/// <param name="vertices">:	制御点の配列	</param>
	/// <param name="t">:			間隔 0~1		</param>
	/// <returns>:					描画に使う点	</returns>
	Vec3 CaclationSpline(const std::vector<Vec3>& vertices, float t);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="vertices"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	Vec3 SplineInterpolation(const std::array<Vec3, 4>& vertices, float t);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	uint32_t          segmentDivisions_ = 12u;
	std::vector<Vec3> anchorPointArray_;
	std::vector<Vec3> segmentPoints_;

	class Line3D*     line3D_           = nullptr;

};