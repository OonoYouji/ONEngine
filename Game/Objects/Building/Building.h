#pragma once

#include <GameObjectManager.h>

class Building final : public BaseGameObject {
public:

	Building()  { CreateTag(this); }
	~Building() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// このオブジェクトの親となるpivot_とその回転
	Transform  pivot_;
	Quaternion quaternionX_ = { 0.0f, 0.0f, 0.0f, 1.0f };
	Quaternion quaternionY_ = { 0.0f, 0.0f, 0.0f, 1.0f };
	float      rotateRateX_ = 0.0f;
	float      rotateRateY_ = 0.0f;

};