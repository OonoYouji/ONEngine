#pragma once

#include "GameObjectManager/GameObjectManager.h"

class PlayerBullet : public BaseGameObject {
public:

    PlayerBullet();
    ~PlayerBullet();

    void Initialize() override;
    void Update()     override;

private:

};
