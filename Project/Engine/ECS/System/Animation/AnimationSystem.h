#pragma once

/// engine
#include "../Interface/ECSISystem.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// アニメーションを更新するシステム
/// ///////////////////////////////////////////////////
/**
 * @class AnimationSystem
 * @brief 個々のエンティティが持つアニメーション再生（AnimationPlayer等）状態を進め、毎フレーム更新するシステムクラス
 */
class AnimationSystem : public ECSISystem {
public:
    /**
     * @brief コンストラクタ
     */
    AnimationSystem() = default;

    /**
     * @brief デストラクタ
     */
    ~AnimationSystem() override = default;

    /**
     * @brief エディタ停止中（非実行時）のアニメーション更新処理を行います。
     * @param _ecs 対象となるECSグループへのポインタ
     */
    void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

    /**
     * @brief ランタイム実行時のアニメーション更新処理を行います。
     * @param _ecs 対象となるECSグループへのポインタ
     */
    void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
    /**
     * @brief 指定デルタ時間に基づき、アニメーションの再生処理を進める共通の内部更新関数です。
     * @param _ecs 対象となるECSグループへのポインタ
     * @param _deltaTime フレーム更新の経過時間（秒）
     */
    void Update(class ECSGroup* _ecs, float _deltaTime);
};

} /// namespace ONEngine
