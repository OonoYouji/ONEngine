#pragma once

#include "../Interface/ECSISystem.h"

namespace ONEngine {

class AISystem : public ECSISystem {
public:
    AISystem();
    ~AISystem() override = default;

    void RuntimeUpdate(class ECSGroup* _ecs) override;
};

}
