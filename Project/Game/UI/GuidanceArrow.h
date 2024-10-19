#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>


//std
#include<optional>
#include<memory>

class Boss;
class Player;
class GuidanceArrow : public BaseGameObject {
public:

	GuidanceArrow() { CreateTag(this); }
	~GuidanceArrow() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;
	
	//setter
	void SetBoss(Boss* boss);
	void SetPlayer(Player* player);

	//getter
	Transform* GetPivot() { return &pivot_; }
	/*Transform* GetbaseTransform() { return &transoform_; }*/

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// other class pointer
	Boss* pBoss_;
	Player* pPlayer_;

	//クォータニオンRotate
	Quaternion rotateX_;
	Quaternion rotateY_;
	//ピボット
	Transform pivot_;



};