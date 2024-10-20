#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
//std
#include<memory>
#include <array>
//obj 
#include"Objects/Tornado/Tornado.h"


class InTornadoEnemy : public BaseGameObject {
public:

	InTornadoEnemy() { CreateTag(this); }
	~InTornadoEnemy() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;
	//getter
	bool GetIsDeath()const { return isDeath_; }
	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision);
	void SetOriginalPos(const std::pair<float, float>& pos) { originalPos = pos; }

	//setter
	void SetTornado(Tornado* tornade);
	void SetIsDeath(bool is);
	const std::pair<float, float>& GetOriginalPos() const { return originalPos; }


private:

	bool isDeath_;
	float ofsetX;
	float ofsetY;
	float speed_;
	Vec2 radius_;
	//
	float theta_;
	//
	float phi_;

	float maxDebuf_;
	const float maxSpeed_ = 4.5f;
	std::pair<float, float> originalPos;

	Tornado* pTornado_ = nullptr;
};