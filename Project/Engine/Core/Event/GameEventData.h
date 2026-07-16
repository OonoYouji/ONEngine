#pragma once

#include <string>
#include <vector>
#include <map>
#include "Engine/Core/Utility/Math/Vector3.h"

namespace ONEngine {

    /// @brief 攻撃のパラメータを定義する構造体
    struct AttackDefinition {
        std::string name;
        float damage = 10.0f;
        float radius = 2.0f;
        float duration = 0.1f;
        Vector3 offsetForward = { 0, 0, 1.0f };
        Vector3 offsetUp = { 0, 1.0f, 0 };
    };

    /// @brief アニメーション再生のパラメータを定義する構造体
    struct AnimationDefinition {
        std::string name;
        std::string clipName;
        bool loop = false;
        float speed = 1.0f;
        float crossFadeTime = 0.1f;
    };

    /// @brief エフェクト（パーティクル等）のパラメータを定義する構造体
    struct EffectDefinition {
        std::string name;
        std::string effectPath; // エフェクトリソース（.prefab等）へのパス
        float duration = 2.0f;
        float defaultScale = 1.0f;
    };

    /// @brief ゲーム内の各種イベントデータを管理するクラス
    class GameEventManager {
    public:
        /**
         * @brief シングルトンインスタンスを取得します。
         * @return GameEventManagerインスタンスの参照
         */
        static GameEventManager& GetInstance();

        /**
         * @brief 各種データをクリアして初期状態にします。
         */
        void Initialize();

        /**
         * @brief JSONファイル（GameEvents.json）から設定データをロードします。
         */
        void Load();

        /**
         * @brief 現在保持している設定データをJSONファイルに保存します。
         */
        void Save();

        // 攻撃データの操作
        /**
         * @brief 指定された名前の攻撃パラメータ定義を取得します。
         * @param _name 攻撃名
         * @return 攻撃定義ポインタ（見つからない場合は nullptr）
         */
        const AttackDefinition* GetAttack(const std::string& _name) const;

        /**
         * @brief 登録されているすべての攻撃定義マップを取得します。
         * @return 攻撃定義マップの参照
         */
        std::map<std::string, AttackDefinition>& GetAttacks() { return attacks_; }

        /**
         * @brief 攻撃定義を追加または更新します。
         * @param _attack 追加する攻撃定義
         */
        void AddAttack(const AttackDefinition& _attack);

        /**
         * @brief 指定された名前の攻撃定義を削除します。
         * @param _name 攻撃名
         */
        void RemoveAttack(const std::string& _name);

        // アニメーションデータの操作
        /**
         * @brief 指定された名前のアニメーションパラメータ定義を取得します。
         * @param _name アニメーション名
         * @return アニメーション定義ポインタ（見つからない場合は nullptr）
         */
        const AnimationDefinition* GetAnimation(const std::string& _name) const;

        /**
         * @brief 登録されているすべてのアニメーション定義マップを取得します。
         * @return アニメーション定義マップの参照
         */
        std::map<std::string, AnimationDefinition>& GetAnimations() { return animations_; }

        /**
         * @brief アニメーション定義を追加または更新します。
         * @param _anim 追加するアニメーション定義
         */
        void AddAnimation(const AnimationDefinition& _anim);

        /**
         * @brief 指定された名前のアニメーション定義を削除します。
         * @param _name アニメーション名
         */
        void RemoveAnimation(const std::string& _name);

        // エフェクトデータの操作
        /**
         * @brief 指定された名前のエフェクトパラメータ定義を取得します。
         * @param _name エフェクト名
         * @return エフェクト定義ポインタ（見つからない場合は nullptr）
         */
        const EffectDefinition* GetEffect(const std::string& _name) const;

        /**
         * @brief 登録されているすべてのエフェクト定義マップを取得します。
         * @return エフェクト定義マップの参照
         */
        std::map<std::string, EffectDefinition>& GetEffects() { return effects_; }

        /**
         * @brief エフェクト定義を追加または更新します。
         * @param _effect 追加するエフェクト定義
         */
        void AddEffect(const EffectDefinition& _effect);

        /**
         * @brief 指定された名前のエフェクト定義を削除します。
         * @param _name エフェクト名
         */
        void RemoveEffect(const std::string& _name);

        /**
         * @brief 設定が変更され、保存が必要な状態（Dirty）か判定します。
         * @return 保存が必要なら true
         */
        bool IsDirty() const { return isDirty_; }

        /**
         * @brief 保存が必要な状態（Dirty）フラグを設定します。
         * @param _dirty 設定するフラグ値
         */
        void SetDirty(bool _dirty) { isDirty_ = _dirty; }

    private:
        GameEventManager() = default;
        ~GameEventManager() = default;

        std::map<std::string, AttackDefinition> attacks_;
        std::map<std::string, AnimationDefinition> animations_;
        std::map<std::string, EffectDefinition> effects_;

        bool isDirty_ = false;

        static constexpr const char* kConfigPath = "Assets/Jsons/GameEvents.json";
    };

}
