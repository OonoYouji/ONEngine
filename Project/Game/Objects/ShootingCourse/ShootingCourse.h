#pragma once

#include "GameObjectManager/GameObjectManager.h"

class ShootingCourse : BaseGameObject {
public:

    ShootingCourse();
    ~ShootingCourse();

    void Initialize() override;
    void Update()     override;

private:

};
