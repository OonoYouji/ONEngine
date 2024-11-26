#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ButtonBUI : public BaseGameObject {
public:

    /// ===================================================
    /// public : methods
    /// ===================================================

    ButtonBUI();
    ~ButtonBUI();

    void Initialize() override;
    void Update()     override;

private:

    /// ===================================================
    /// private : objects
    /// ===================================================

};
