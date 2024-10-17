#pragma once

/// std
#include <vector>

#include "GameObjectManager/GameObjectManager.h"
#include <ComponentManager/AudioSource/AudioSource.h>
#include <ComponentManager/SpriteRenderer/SpriteRenderer.h>

/// ===================================================
/// 前方宣言
/// ===================================================

/// ===================================================
/// チュートリアルUIクラス
/// ===================================================
class BaseTutorialUI:public BaseGameObject {
public:
	BaseTutorialUI() { CreateTag(this);}
	~BaseTutorialUI() {}

	virtual	void Initialize() override;
	virtual	void Update()     override;
	virtual	void Debug()      override;

	//getter
	bool GetIsColse()const { return isClose_; }
	bool GetIsDeath()const { return isDeath_; }
	//setter
	void SetIsClose(bool is){isClose_=is; }
private:
	bool isClose_;
	bool canClose_;
	float easeT_;
	float easeCollTime_;
	const float kEaseTime_ = 0.5f;
	bool isDeath_;
};

class TutorialScaleUpUI  : public BaseTutorialUI {
public:

	TutorialScaleUpUI() { CreateTag(this); }
	~TutorialScaleUpUI() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:



};