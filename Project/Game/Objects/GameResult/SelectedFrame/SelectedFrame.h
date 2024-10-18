#pragma once

#include "GameObjectManager/GameObjectManager.h"

class SelectedFrame : public BaseGameObject {
public:

    SelectedFrame();
    ~SelectedFrame();

    void Initialize() override;
    void Update()     override;

private:

};
