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

private:

	std::vector<IOData> ioDataArray_;

};
