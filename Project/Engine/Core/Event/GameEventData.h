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
        static GameEventManager& GetInstance();

        void Initialize();
        void Load();
        void Save();

        // 攻撃データの操作
        const AttackDefinition* GetAttack(const std::string& _name) const;
        std::map<std::string, AttackDefinition>& GetAttacks() { return attacks_; }
        void AddAttack(const AttackDefinition& _attack);
        void RemoveAttack(const std::string& _name);

        // アニメーションデータの操作
        const AnimationDefinition* GetAnimation(const std::string& _name) const;
        std::map<std::string, AnimationDefinition>& GetAnimations() { return animations_; }
        void AddAnimation(const AnimationDefinition& _anim);
        void RemoveAnimation(const std::string& _name);

        // エフェクトデータの操作
        const EffectDefinition* GetEffect(const std::string& _name) const;
        std::map<std::string, EffectDefinition>& GetEffects() { return effects_; }
        void AddEffect(const EffectDefinition& _effect);
        void RemoveEffect(const std::string& _name);

        bool IsDirty() const { return isDirty_; }
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
