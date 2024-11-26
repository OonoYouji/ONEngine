#pragma once

#include "GameObjectManager/BaseGameObject.h"

class UIManager : public BaseGameObject {
public:

    /// ===================================================
    /// public : methods
    /// ===================================================

    UIManager();
    ~UIManager();

    void Initialize() override;
    void Update()     override;

private:

    /// ===================================================
    /// private : objects
    /// ===================================================

};
