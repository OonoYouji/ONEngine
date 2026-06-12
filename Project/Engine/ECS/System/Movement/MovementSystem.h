#pragma once

#include "../Interface/ECSISystem.h"

namespace ONEngine {

class MovementSystem : public ECSISystem {
public:
    MovementSystem();
    ~MovementSystem() override = default;

    void RuntimeUpdate(class ECSGroup* _ecs) override;
};

}
