#pragma once

#include "../Interface/ECSISystem.h"

namespace ONEngine {

/**
 * @class MovementSystem
 * @brief AIの移動意志（AgentIntentComponent）や物理的な速度・加速度に基づき、エンティティの座標（Transform）を更新するシステムクラス
 */
class MovementSystem : public ECSISystem {
public:
    /**
     * @brief コンストラクタ
     */
    MovementSystem();

    /**
     * @brief デストラクタ
     */
    ~MovementSystem() override = default;

    /**
     * @brief 毎フレームの移動計算および位置更新処理を実行します。
     * @param _ecs 対象となるECSグループへのポインタ
     */
    void RuntimeUpdate(class ECSGroup* _ecs) override;
};

}
