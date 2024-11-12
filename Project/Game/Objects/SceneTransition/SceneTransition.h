#pragma once

#include "GameObjectManager/BaseGameObject.h"

enum TRANSITION_TYPE {
	TRANSITION_TYPE_IN,	  /// フェードイン
	TRANSITION_TYPE_OUT,  /// フェードアウト
	TRANSITION_TYPE_COUNT /// カウント用
};


class SceneTransition : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SceneTransition(TRANSITION_TYPE _type, float _animationTime, uint32_t _drawLayerId);
	~SceneTransition();

	void Initialize() override;
	void Update()     override;


	void TransitionIn();
	void TransitionOut();

	/// <summary>
	/// 終了フラグのゲット
	/// </summary>
	/// <returns></returns>
	bool GetIsEnd() const { return isEnd_; }

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* spriteRenderer_ = nullptr;
	
	TRANSITION_TYPE transitionType_;

	float currentAnimationTime_;
	float maxAnimationTime_;

	Vec4 color_;

	bool isEnd_ = false; /// 遷移終了フラグ
};
