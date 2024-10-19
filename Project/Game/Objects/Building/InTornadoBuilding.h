#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
//std
#include<memory>
#include <array>
//obj 
#include"Objects/Tornado/Tornado.h"

class InTornadoBuilding : public BaseGameObject {
public:

	InTornadoBuilding() { CreateTag(this); }
	~InTornadoBuilding() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;
	//getter
	bool GetIsDeath()const { return isDeath_; }
	const int GetScaleArrayIndex()const { return scaleArrayIndex_; }
	//setter
	void SetModel(Model* model);
	void SetTornado(Tornado* tornade);
	void SetIsDeath(bool is);
		

	void SetScaleArrayIndex(int index);

private:
	
	static std::array<Vec3, 3> buildingScaleArray_;


	bool isDeath_;
	float ofsetX;
	float ofsetY;
	float speed_;
	Vec2 radius_;
	//
	float theta_;
	//
	float phi_;

	Tornado* pTornado_ = nullptr;

	float maxDebuf_;
	const float maxSpeed_ = 4.5f;

	int scaleArrayIndex_ = 0;

};