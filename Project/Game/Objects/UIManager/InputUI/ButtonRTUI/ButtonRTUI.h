#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ButtonRTUI : public BaseGameObject {
public:

    /// ===================================================
    /// public : methods
    /// ===================================================

    ButtonRTUI();
    ~ButtonRTUI();

    void Initialize() override;
    void Update()     override;

    void AddVariables();
    void ApplyVariables();

private:

    /// ===================================================
    /// private : objects
    /// ===================================================

    class SpriteRenderer* renderer_ = nullptr;
};