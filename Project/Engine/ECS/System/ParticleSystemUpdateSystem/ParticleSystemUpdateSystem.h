#pragma once

#include "../Interface/ECSISystem.h"

namespace ONEngine {

    /**
     * @class ParticleSystemUpdateSystem
     * @brief パーティクルシステム（ParticleSystem）のシミュレーション（発生、運動、サイズ・カラー変化、ライフタイム管理）およびデバッグ用ギズモの描画を司るシステムクラス
     */
    class ParticleSystemUpdateSystem : public ECSISystem {
    public:
        /**
         * @brief コンストラクタ
         */
        ParticleSystemUpdateSystem();

        /**
         * @brief デストラクタ
         */
        ~ParticleSystemUpdateSystem() override = default;

        /**
         * @brief 毎フレームのパーティクルシミュレーション状態の更新処理を実行します。
         * @param _ecs 対象となるECSグループへのポインタ
         */
        void RuntimeUpdate(class ECSGroup* _ecs) override;

        /**
         * @brief エディタ非実行時のパーティクル更新処理（およびギズモ等の表示）を行います。
         * @param _ecs 対象となるECSグループへのポインタ
         */
        void OutsideOfRuntimeUpdate(class ECSGroup* _ecs) override;

    private:
        /**
         * @brief エディタ用：パーティクルエミッター（形状等）のデバッグ用ギズモを描画します。
         * @param _ecs 対象となるECSグループへのポインタ
         */
        void DrawGizmos(class ECSGroup* _ecs);

        /**
         * @brief 単一の ParticleSystem コンポーネントにおけるシミュレーション計算を1ステップ進めます。
         * @param ps シミュレーションを行うパーティクルシステムコンポーネント
         * @param entity コンポーネントを所有するエンティティ
         * @param dt 経過時間（秒）
         */
        void UpdateSingleSystem(class ParticleSystem* ps, class GameEntity* entity, float dt);
    };

}
