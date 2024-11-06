#pragma once

#include "GameObjectManager/BaseGameObject.h"

class DemoSphereColliderOBJ : public BaseGameObject {
public:

    /// ===================================================
    /// public : methods
    /// ===================================================

    DemoSphereColliderOBJ();
    ~DemoSphereColliderOBJ();

    void Initialize() override;
    void Update()     override;

private:

    /// ===================================================
    /// private : objects
    /// ===================================================

};
