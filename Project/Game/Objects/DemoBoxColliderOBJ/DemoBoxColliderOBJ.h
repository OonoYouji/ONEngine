#pragma once

#include "GameObjectManager/BaseGameObject.h"

class DemoBoxColliderOBJ : public BaseGameObject {
public:

    /// ===================================================
    /// public : methods
    /// ===================================================

    DemoBoxColliderOBJ();
    ~DemoBoxColliderOBJ();

    void Initialize() override;
    void Update()     override;

private:

    /// ===================================================
    /// private : objects
    /// ===================================================

};
