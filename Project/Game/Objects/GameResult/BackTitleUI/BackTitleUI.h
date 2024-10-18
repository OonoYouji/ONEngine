#pragma once

#include "GameObjectManager/GameObjectManager.h"

class BackTitleUI : public BaseGameObject {
public:

    BackTitleUI();
    ~BackTitleUI();

    void Initialize() override;
    void Update()     override;

private:
    class SpriteRenderer* spriteRenderer_ = nullptr;
};
