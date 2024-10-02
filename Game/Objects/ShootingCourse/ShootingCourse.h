#pragma once

#include <GameObjectManager.h>

#include <Component/SplinePathRenderer/SplinePathRenderer.h>


class ShootingCourse final : public BaseGameObject {

	struct AnchorPoitnt {
		Vec3 position;
		Vec3 up;
	};

public:

	ShootingCourse() { CreateTag(this); }
	~ShootingCourse() {}

	void Initialize() override;
	void Update()     override;

private:

	SplinePathRenderer*       splinePathRenderer_ = nullptr;
	std::vector<AnchorPoitnt> anchorPointArray_;


};