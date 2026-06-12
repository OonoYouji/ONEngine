#pragma once

/// engine
#include "../Interface/ECSISystem.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// アニメーションを更新するシステム
/// ///////////////////////////////////////////////////
class AnimationSystem : public ECSISystem {
public:
    AnimationSystem() = default;
    ~AnimationSystem() override = default;

    void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;
    void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
    void Update(class ECSGroup* _ecs, float _deltaTime);
};

} /// namespace ONEngine
