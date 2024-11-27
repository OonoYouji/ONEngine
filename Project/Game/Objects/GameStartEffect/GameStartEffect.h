#pragma once

#include "GameObjectManager/BaseGameObject.h"

class GameStartEffect : public BaseGameObject {
public:

    /// ===================================================
    /// public : methods
    /// ===================================================

    GameStartEffect();
    ~GameStartEffect();

    void Initialize() override;
    void Update()     override;

private:

    /// ===================================================
    /// private : objects
    /// ===================================================

};
