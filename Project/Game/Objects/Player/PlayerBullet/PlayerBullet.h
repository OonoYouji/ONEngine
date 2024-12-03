#pragma once

#include "GameObjectManager/BaseGameObject.h"

class PlayerBullet : public BaseGameObject {
public:

    /// ===================================================
    /// public : methods
    /// ===================================================

    PlayerBullet();
    ~PlayerBullet();

    void Initialize() override;
    void Update()     override;

private:

    /// ===================================================
    /// private : objects
    /// ===================================================

};
