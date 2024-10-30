#pragma once

/// std
#include <string>
#include <vector>
#include <list>

#include "GameObjectManager/GameObjectManager.h"

#include "ComponentManager/Transform/Transform.h"


/// ===================================================
/// シューティングコースのアンカーポイント
/// ===================================================
struct AnchorPoint {
	Vec3 position;
	Vec3 up = Vec3::kUp;
};


/// ===================================================
/// シューティングコースのエディター含めたクラス
/// ===================================================
class ShootingCourse : BaseGameObject {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ShootingCourse();
	~ShootingCourse();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	const std::vector<AnchorPoint>& GetAnchorPointArray() const {
		return anchorPointArray_;
	}


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void SaveFile(const std::string& filePath);
	void LoadFile(const std::string& filePath);

	void AddAnchorPoint(const Vec3& point);


	void CalcuationRailTransform();

	void CalcuationAnchorPointArray();

	void CalcuationUpDirctionArray();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	const std::string& filePath_ = "./Resources/Parameters/ShootingCourse/";


	class MeshInstancingRenderer* meshInstancedRenderer_ = nullptr;
	class MeshInstancingRenderer* anchorPointRenderer_      = nullptr;

	std::vector<AnchorPoint> anchorPointArray_;
	std::list<Transform>     transformList_;
	std::list<Transform>     anchorPointTransformList_;

	std::vector<Vec3> vertices_; /// spline path rendererに渡す用
	std::vector<Vec3> upDirArray_; /// 上方向ベクトルを補間するため
	class SplinePathRenderer* splinePathRenderer_ = nullptr;
	class SplinePathRenderer* upDirInterpolationRenderer_ = nullptr;

	int subtractIndex_ = 0;
	int addIndex_ = 0;

};




/// <summary>
/// アンカーポイントの配列と現在の位置を表す媒介変数で現在地のワールド座標を計算する
/// </summary>
/// <param name="anchorPointArray">: アンカーポイントの配列 </param>
/// <param name="t">               : 現在地を表す媒介変数   </param>
/// <returns>return                : 現在地のワールド座標   </returns>
AnchorPoint SplinePosition(const std::vector<AnchorPoint>& anchorPointArray, float t);

/// <summary>
/// スプライン曲線の座標を補完する
/// </summary>
/// <param name=""></param>
/// <returns></returns>
AnchorPoint SplineInterpolation(const std::array<AnchorPoint, 4>& anchorPointArray, float t);
