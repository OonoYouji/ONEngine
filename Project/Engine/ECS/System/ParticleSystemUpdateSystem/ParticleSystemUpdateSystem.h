#pragma once

#include "../Interface/ECSISystem.h"

namespace ONEngine {

    class ParticleSystemUpdateSystem : public ECSISystem {
    public:
        ParticleSystemUpdateSystem();
        ~ParticleSystemUpdateSystem() override = default;

        void RuntimeUpdate(class ECSGroup* _ecs) override;
        void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

    private:
        void DrawGizmos(class ECSGroup* _ecs);
        void UpdateSingleSystem(class ParticleSystem* ps, class GameEntity* entity, float dt);
    };

}
