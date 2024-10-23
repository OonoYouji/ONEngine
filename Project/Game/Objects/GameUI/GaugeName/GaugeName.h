#pragma once

#include "GameObjectManager/GameObjectManager.h"

class GaugeName : public BaseGameObject {
public:

    GaugeName();
    ~GaugeName();

    void Initialize() override;
    void Update()     override;

private:

};
