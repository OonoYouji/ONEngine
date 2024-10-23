#pragma once

#include "GameObjectManager/GameObjectManager.h"

class PowerUpGauge : public BaseGameObject {
public:

    PowerUpGauge();
    ~PowerUpGauge();

    void Initialize() override;
    void Update()     override;

private:

};
