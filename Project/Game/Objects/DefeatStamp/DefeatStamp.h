#pragma once

#include "GameObjectManager/GameObjectManager.h"

class DefeatStamp : public BaseGameObject {
public:

    DefeatStamp();
    ~DefeatStamp();

    void Initialize() override;
    void Update()     override;

private:

};
