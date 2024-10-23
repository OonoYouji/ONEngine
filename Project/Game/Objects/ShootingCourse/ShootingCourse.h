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


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	const std::string& filePath_ = "./Resources/Parameters/ShootingCourse/";


	class MeshInstancingRenderer* meshInstancedRenderer_ = nullptr;

	std::vector<AnchorPoint> anchorPointArray_;
	std::list<Transform>     transformList_;

	std::vector<Vec3> vertices_; /// spline path rendererに渡す用
	class SplinePathRenderer* splinePathRenderer_ = nullptr;

	int subtractIndex_ = 0;
	int addIndex_ = 0;

};
