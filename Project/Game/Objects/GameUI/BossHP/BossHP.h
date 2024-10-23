#pragma once

#include "GameObjectManager/GameObjectManager.h"

class Boss;

class BossHP : public BaseGameObject {
public:

    BossHP();
    ~BossHP();

    void Initialize() override;
    void Update()     override;
    void Debug()      override;


    void SetBoss(Boss* _boss);

private:
    class MeshRenderer* mr_ = nullptr;
    class Boss* pBoss_ = nullptr;

    Vec2 uvPosition_;
    Vec2 uvScale_;

};
