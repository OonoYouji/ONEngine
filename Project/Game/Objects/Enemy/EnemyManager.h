#pragma once


/// std
#include <vector>

/// objects
#include "Enemy.h"

/// base class
#include "GameObjectManager/GameObjectManager.h"

class EnemyManager : public BaseGameObject {

	struct IOData {
		Vec3  startOffset;
		float startedT;
		float hp;
	};

public:

	EnemyManager();
	~EnemyManager();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void CreateEnemy(
		const Vec3& _scale,
		const Vec3& _rotate, 
		const Vec3& _position,
		float _updateStartT,
		float _hp
	) const;

	void SaveFile(const std::string& filePath);
	void LoadFile(const std::string& filePath);



	void CalcuationEnemyStartedAnchorPoint();



	void SetShootingCourse(class ShootingCourse* _shootingCourse);
	void SetRailCamera(class RailCamera* _railCamera);



private:

	class MeshInstancingRenderer* meshInstancingRenderer_ = nullptr; /// enemyが更新されるタイミングのポイント描画


	std::vector<IOData> ioDataArray_;
	std::vector<IOData> enemyCreateDataArray_;
	std::vector<Transform> startedTTransforms_;


	/// other class pointer
	class ShootingCourse* pShootingCourse_ = nullptr;
	class RailCamera*     pRailCamera_     = nullptr;

};
