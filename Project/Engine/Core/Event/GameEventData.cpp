#include "GameEventData.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "Engine/Core/Utility/Utility.h"

using json = nlohmann::json;

namespace ONEngine {

    GameEventManager& GameEventManager::GetInstance() {
        static GameEventManager instance;
        return instance;
    }

    void GameEventManager::Initialize() {
        static bool initialized = false;
        if (initialized) return;
        
        Load();
        initialized = true;
    }

    void GameEventManager::Load() {
        std::ifstream file(kConfigPath);
        if (!file.is_open()) return;

        json data;
        try {
            file >> data;
        } catch (...) {
            return;
        }

        attacks_.clear();
        if (data.contains("attacks")) {
            for (auto& item : data["attacks"]) {
                AttackDefinition def;
                def.name = item["name"];
                def.damage = item["damage"];
                def.radius = item["radius"];
                def.duration = item["duration"];
                def.offsetForward = Vector3(item["offsetForward"][0].get<float>(), item["offsetForward"][1].get<float>(), item["offsetForward"][2].get<float>());
                def.offsetUp = Vector3(item["offsetUp"][0].get<float>(), item["offsetUp"][1].get<float>(), item["offsetUp"][2].get<float>());
                attacks_[def.name] = def;
            }
        }

        animations_.clear();
        if (data.contains("animations")) {
            for (auto& item : data["animations"]) {
                AnimationDefinition def;
                def.name = item["name"];
                def.clipName = item["clipName"];
                def.loop = item["loop"];
                def.speed = item["speed"];
                def.crossFadeTime = item["crossFadeTime"];
                animations_[def.name] = def;
            }
        }

        effects_.clear();
        if (data.contains("effects")) {
            for (auto& item : data["effects"]) {
                EffectDefinition def;
                def.name = item["name"];
                def.effectPath = item["effectPath"];
                def.duration = item["duration"];
                def.defaultScale = item["defaultScale"];
                effects_[def.name] = def;
            }
        }

        isDirty_ = false;
    }

    void GameEventManager::Save() {
        json data;
        
        data["attacks"] = json::array();
        for (auto& [name, def] : attacks_) {
            json item;
            item["name"] = def.name;
            item["damage"] = def.damage;
            item["radius"] = def.radius;
            item["duration"] = def.duration;
            item["offsetForward"] = json::array({ def.offsetForward.x, def.offsetForward.y, def.offsetForward.z });
            item["offsetUp"] = json::array({ def.offsetUp.x, def.offsetUp.y, def.offsetUp.z });
            data["attacks"].push_back(item);
        }

        data["animations"] = json::array();
        for (auto& [name, def] : animations_) {
            json item;
            item["name"] = def.name;
            item["clipName"] = def.clipName;
            item["loop"] = def.loop;
            item["speed"] = def.speed;
            item["crossFadeTime"] = def.crossFadeTime;
            data["animations"].push_back(item);
        }

        data["effects"] = json::array();
        for (auto& [name, def] : effects_) {
            json item;
            item["name"] = def.name;
            item["effectPath"] = def.effectPath;
            item["duration"] = def.duration;
            item["defaultScale"] = def.defaultScale;
            data["effects"].push_back(item);
        }

        std::ofstream file(kConfigPath);
        if (file.is_open()) {
            file << data.dump(4);
            isDirty_ = false;
        }
    }

    const AttackDefinition* GameEventManager::GetAttack(const std::string& _name) const {
        auto it = attacks_.find(_name);
        if (it != attacks_.end()) return &it->second;
        return nullptr;
    }

    void GameEventManager::AddAttack(const AttackDefinition& _attack) {
        attacks_[_attack.name] = _attack;
    }

    void GameEventManager::RemoveAttack(const std::string& _name) {
        attacks_.erase(_name);
    }

    const AnimationDefinition* GameEventManager::GetAnimation(const std::string& _name) const {
        auto it = animations_.find(_name);
        if (it != animations_.end()) return &it->second;
        return nullptr;
    }

    void GameEventManager::AddAnimation(const AnimationDefinition& _anim) {
        animations_[_anim.name] = _anim;
    }

    void GameEventManager::RemoveAnimation(const std::string& _name) {
        animations_.erase(_name);
    }

    const EffectDefinition* GameEventManager::GetEffect(const std::string& _name) const {
        auto it = effects_.find(_name);
        if (it != effects_.end()) return &it->second;
        return nullptr;
    }

    void GameEventManager::AddEffect(const EffectDefinition& _effect) {
        effects_[_effect.name] = _effect;
    }

    void GameEventManager::RemoveEffect(const std::string& _name) {
        effects_.erase(_name);
    }

}
