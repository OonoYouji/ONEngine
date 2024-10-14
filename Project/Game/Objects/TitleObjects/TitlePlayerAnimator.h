#pragma once

#include "GameObjectManager/BaseGameObject.h"

class TitlePlayerAnimator final : public BaseGameObject {
public:

	TitlePlayerAnimator() { CreateTag(this); }
	~TitlePlayerAnimator() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

};
