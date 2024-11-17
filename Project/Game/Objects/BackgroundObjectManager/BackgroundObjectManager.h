#pragma once

/// std
#include <memory>
#include <vector>

/// objects
#include "BaseBackgroundObject/BaseBackgroundObject.h"

/// base class
#include "GameObjectManager/BaseGameObject.h"


class BackgroundObjectManager : public BaseGameObject {

	struct ObjectData {
		Vec3 offsetPosition;
		float startedT;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BackgroundObjectManager();
	~BackgroundObjectManager();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void CalcuationObjectDataStartedTransform();

	void SaveJsonFile(const std::string& _filePath);
	void LoadJsonFile(const std::string& _filePath);



	void SetShootingCourse(class ShootingCourse* _shootingCourse);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	int operatorIndex_;
	ObjectData                         srcObjectData_;
	std::vector<ObjectData>            objectDataArray_;
	std::vector<BaseBackgroundObject*> backgroundObjectArray_;

	std::vector<Transform> objectDataStartedTTransformArray_;
	class MeshInstancingRenderer* startedTRenderer_ = nullptr;

	class ShootingCourse* pShootingCourse_ = nullptr;

};
