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
		std::string createClassName = "MovingLight";
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

	void PopBBObject();

	void SaveJsonFile(const std::string& _filePath);
	void LoadJsonFile(const std::string& _filePath);


	template <typename T>
	void CreateObject(const Vec3& wPosition);

	void CreateBBObject(const std::string& _className, const Vec3& wPosition);


	void SetShootingCourse(class ShootingCourse* _shootingCourse);
	void SetRailCamera(class RailCamera* _railCamera);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	int operatorIndex_;
	int currentStringIndex_;
	std::vector<std::string>           createClassNameArray_;
	ObjectData                         srcObjectData_;
	std::vector<ObjectData>            objectDataArray_;
	std::vector<ObjectData>            popObjectDataArray_;
	std::vector<BaseBackgroundObject*> backgroundObjectArray_;

	std::vector<Transform>        objectDataStartedTTransformArray_;
	class MeshInstancingRenderer* startedTRenderer_ = nullptr;

	class ShootingCourse* pShootingCourse_ = nullptr;
	class RailCamera*     pRailCamera_     = nullptr;

};



template<typename T>
inline void BackgroundObjectManager::CreateObject(const Vec3& wPosition) {
	static_assert(std::is_base_of<BaseBackgroundObject, T>::value, "T must inherit from BaseBackgroundObject");

	T* t = new T(wPosition);
	t->Initialize();
	backgroundObjectArray_.push_back(t);
}
