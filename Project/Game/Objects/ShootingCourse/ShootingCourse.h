#pragma once

#include <string>
#include <vector>

#include "GameObjectManager/GameObjectManager.h"


/// ===================================================
/// シューティングコースのエディター含めたクラス
/// ===================================================
class ShootingCourse : BaseGameObject {

	struct AnchorPoint {
		Vec3 poision;
		float twist;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ShootingCourse();
	~ShootingCourse();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void SaveFile(const std::string& filePath);
	void LoadFile(const std::string& filePath);

	void AddAnchorPoint(const Vec3& point);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	const std::string& filePath_ = "./Resources/Parameters/ShootingCourse/";

	std::vector<AnchorPoint> anchorPointArray_;

	std::vector<Vec3> vertices_; /// spline path rendererに渡す用
	class SplinePathRenderer* splinePathRenderer_ = nullptr;

	int subtractIndex_ = 0;
	int addIndex_ = 0;

};
