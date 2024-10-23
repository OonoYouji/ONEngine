#pragma once

#include "GameObjectManager/GameObjectManager.h"

class BossName : public BaseGameObject {
public:

    BossName();
    ~BossName();

    void Initialize() override;
    void Update()     override;

private:

};
