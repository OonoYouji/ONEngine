#pragma once


#include <list>


#include "GameObjectManager/BaseGameObject.h"

class EnemyManager : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	EnemyManager();
	~EnemyManager();

	void Initialize() override;
	void Update()     override;


	/// <summary>
	/// Enemyを生成する
	/// </summary>
	/// <param name="_position"></param>
	void GenerateEnemy(const Vec3& _position);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<class Enemy*> enemyList_;

};
