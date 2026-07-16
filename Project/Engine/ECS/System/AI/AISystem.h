#pragma once

#include "../Interface/ECSISystem.h"

namespace ONEngine {

/**
 * @class AISystem
 * @brief ゲーム内のAI機能（思考ルーチンや意思決定プロセスなど）を毎フレーム更新・制御するシステムクラス
 */
class AISystem : public ECSISystem {
public:
    /**
     * @brief コンストラクタ
     */
    AISystem();

    /**
     * @brief デストラクタ
     */
    ~AISystem() override = default;

    /**
     * @brief AIの意思決定および思考処理の更新処理を毎フレーム実行します。
     * @param _ecs 対象となるECSグループへのポインタ
     */
    void RuntimeUpdate(class ECSGroup* _ecs) override;
};

}
