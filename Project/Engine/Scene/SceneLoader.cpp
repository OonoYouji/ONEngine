#include "SceneLoader.h"
#include "Externals/nlohmann/json.hpp"
#include "Engine/Common/Console.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Script/ScriptHost.h"
#include "Schema/Buffers.h"
#include "Schema/Components.h"
#include <fstream>
#include <filesystem>
#include <map>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace Engine::Scene {

namespace {
    void(*gAddScriptDelegate)(uint32_t, const char*, const char*) = nullptr;

    void EnsureScriptDelegate() {
        if (gAddScriptDelegate) return;
        auto& host = Engine::Script::ScriptHost::GetInstance();
        gAddScriptDelegate = (void(*)(uint32_t, const char*, const char*))host.GetMethodDelegate(
            L"ONEngine.Scripting.EngineHost, ONEngine.Scripting",
            L"AddScriptByName",
            L"");
    }

    void DeserializeEntity(const json& jEntity, Engine::ECS::Registry& registry, std::map<int, Engine::ECS::Entity>& idMap) {
        auto entity = registry.CreateEntity();
        
        // シーンファイル内のIDと実際のEntityハンドルを記録
        if (jEntity.contains("id")) {
            idMap[jEntity["id"]] = entity;
        }

        auto& componentRegistry = Engine::ECS::ComponentRegistry::GetInstance();
        
        if (jEntity.contains("components") && jEntity["components"].is_array()) {
            for (const auto& jComp : jEntity["components"]) {
                if (!jComp.contains("type") || !jComp["type"].is_string()) continue;
                std::string type = jComp["type"];
                
                const auto* info = componentRegistry.GetInfo(type);
                if (info) {
                    info->deserializeFunc(jComp, entity, registry);
                } 
                else if (type == "Script") {
                    EnsureScriptDelegate();
                    if (gAddScriptDelegate && jComp.contains("scripts") && jComp["scripts"].is_array()) {
                        for (const auto& jScript : jComp["scripts"]) {
                            if (jScript.contains("name") && jScript["name"].is_string()) {
                                std::string scriptName = jScript["name"];
                                std::string varsJson = "{}";
                                if (jScript.contains("variables") && jScript["variables"].is_object()) {
                                    varsJson = jScript["variables"].dump();
                                }
                                gAddScriptDelegate(entity, scriptName.c_str(), varsJson.c_str());
                            }
                        }
                    }
                }
            }
        }

        // 親子関係の解決（後で一括で行うために一旦記録するアプローチもあるが、
        // JSON内の ID が数値であれば、この DeserializeEntity を抜けた後に親IDをハンドルに置換する）
    }
}

bool SceneLoader::LoadScene(const std::string& path, Engine::ECS::Registry& registry) {
    Engine::Console::Log(std::format("SceneLoader: Loading scene from {}", path));
    
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        Engine::Console::LogError(std::format("SceneLoader: Failed to open file: {}. Current path: {}", path, std::filesystem::current_path().string()));
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.size() >= 3 && (unsigned char)content[0] == 0xEF && (unsigned char)content[1] == 0xBB && (unsigned char)content[2] == 0xBF) content.erase(0, 3);

    try {
        json data = json::parse(content);
        std::map<int, Engine::ECS::Entity> idMap;
        std::vector<std::pair<Engine::ECS::Entity, int>> parentTasks;

        if (data.contains("entities") && data["entities"].is_array()) {
            size_t entityCount = data["entities"].size();
            Engine::Console::Log(std::format("SceneLoader: Found {} entities in JSON.", entityCount));

            for (const auto& jEntity : data["entities"]) {
                auto entity = registry.CreateEntity();
                int sceneId = jEntity.value("id", -1);
                std::string entityLogName = std::format("Entity(ID:{})", sceneId);

                if (sceneId != -1) idMap[sceneId] = entity;

                // Tagコンポーネントの自動付与（名前とActive状態の同期）
                {
                    auto& tag = registry.AddComponent<Engine::ECS::Tag>(entity);
                    tag.isActive = 1; // デフォルトは有効
                    if (jEntity.contains("name") && jEntity["name"].is_string()) {
                        std::string name = jEntity["name"];
                        size_t len = (std::min)(name.length(), sizeof(tag.name) - 1);
                        std::memcpy(tag.name, name.c_str(), len);
                        tag.name[len] = '\0';
                    } else {
                        sprintf_s(tag.name, "Entity %u", entity);
                    }

                    if (jEntity.contains("isActive")) {
                        tag.isActive = jEntity["isActive"].get<bool>() ? 1 : 0;
                    }
                }

                if (jEntity.contains("parent") && !jEntity["parent"].is_null()) {
                    parentTasks.push_back({entity, jEntity["parent"]});
                }

                auto& componentRegistry = Engine::ECS::ComponentRegistry::GetInstance();
                if (jEntity.contains("components") && jEntity["components"].is_array()) {
                    Engine::Console::Log(std::format("  {}: Processing {} components.", entityLogName, jEntity["components"].size()));
                    for (const auto& jComp : jEntity["components"]) {
                        std::string type = jComp.value("type", "");
                        const auto* info = componentRegistry.GetInfo(type);
                        if (info) {
                            Engine::Console::Log(std::format("    - Loading component '{}'", type));
                            info->deserializeFunc(jComp, entity, registry);
                            
                            // 実際に追加されたか確認
                            if (registry.HasComponent(entity, info->typeId)) {
                                Engine::Console::Log(std::format("      [Success] Component '{}' added to Entity.", type));
                            } else {
                                Engine::Console::LogError(std::format("      [Failed] Component '{}' not found in registry after deserializeFunc!", type));
                            }
                        } else if (type == "Script") {
                            Engine::Console::Log(std::format("  {}: Loading Scripts", entityLogName));
                            EnsureScriptDelegate();
                            if (gAddScriptDelegate && jComp.contains("scripts") && jComp["scripts"].is_array()) {
                                for (const auto& jScript : jComp["scripts"]) {
                                    std::string scriptName = jScript.value("name", "");
                                    std::string varsJson = jScript.contains("variables") ? jScript["variables"].dump() : "{}";
                                    Engine::Console::Log(std::format("    - Script: {}", scriptName));
                                    gAddScriptDelegate(entity, scriptName.c_str(), varsJson.c_str());
                                }
                            }
                        } else {
                            Engine::Console::LogWarning(std::format("  {}: Unknown component type '{}'", entityLogName, type));
                        }
                    }
                }
            }
        }

        Engine::Console::Log(std::format("SceneLoader: Successfully loaded scene with {} entities.", idMap.size()));

        // 親子関係の解決
        for (const auto& task : parentTasks) {
            Engine::ECS::Entity entity = task.first;
            int parentId = task.second;
            if (idMap.count(parentId)) {
                if (registry.HasComponent<Engine::ECS::Transform>(entity)) {
                    registry.GetComponent<Engine::ECS::Transform>(entity).parent = idMap[parentId];
                }
            }
        }

        return true;
    } catch (const std::exception& e) {
        Engine::Console::LogError(std::format("SceneLoader Error: {}", e.what()));
        return false;
    }
}

Engine::ECS::Entity SceneLoader::InstantiatePrefab(const std::string& path, Engine::ECS::Registry& registry) {
    std::ifstream file(path);
    if (!file.is_open()) {
        Engine::Console::LogError(std::format("SceneLoader: Failed to open prefab: {}", path));
        return Engine::ECS::kNullEntity;
    }

    try {
        json jPrefab = json::parse(file);
        auto entity = registry.CreateEntity();

        // Tag
        auto& tag = registry.AddComponent<Engine::ECS::Tag>(entity);
        std::string name = jPrefab.value("name", "Prefab Instance");
        strcpy_s(tag.name, sizeof(tag.name), name.c_str());
        tag.isActive = 1;

        auto& compReg = Engine::ECS::ComponentRegistry::GetInstance();
        if (jPrefab.contains("components") && jPrefab["components"].is_array()) {
            for (const auto& jComp : jPrefab["components"]) {
                if (!jComp.is_object()) continue;
                std::string type = jComp.value("type", "");
                const auto* info = compReg.GetInfo(type);
                if (info) {
                    info->deserializeFunc(jComp, entity, registry);
                } else if (type == "Script") {
                    EnsureScriptDelegate();
                    if (gAddScriptDelegate && jComp.contains("scripts") && jComp["scripts"].is_array()) {
                        for (const auto& jScript : jComp["scripts"]) {
                            if (!jScript.is_object()) continue;
                            std::string scriptName = jScript.value("name", "");
                            std::string varsJson = jScript.contains("variables") ? jScript["variables"].dump() : "{}";
                            gAddScriptDelegate(entity, scriptName.c_str(), varsJson.c_str());
                        }
                    }
                }
            }
        }

        Engine::Console::Log(std::format("SceneLoader: Instantiated prefab from {}", path));
        return entity;
    } catch (const std::exception& e) {
        Engine::Console::LogError(std::format("SceneLoader: Error instantiating prefab {}: {}", path, e.what()));
        return Engine::ECS::kNullEntity;
    }
}

bool SceneLoader::SavePrefab(const std::string& path, Engine::ECS::Entity entity, Engine::ECS::Registry& registry) {
    if (entity == Engine::ECS::kNullEntity) return false;

    try {
        json jPrefab;
        auto& compReg = Engine::ECS::ComponentRegistry::GetInstance();

        if (registry.HasComponent<Engine::ECS::Tag>(entity)) {
            jPrefab["name"] = std::string(registry.GetComponent<Engine::ECS::Tag>(entity).name);
        } else {
            jPrefab["name"] = "Prefab";
        }

        json jComponents = json::array();
        for (auto& [typeId, info] : compReg.GetAll()) {
            if (typeId == 100) continue; // Tag

            if (registry.HasComponent(entity, typeId)) {
                json jComp = compReg.SerializeComponent(registry, entity, typeId);
                
                if (typeId == 3 && jComp.contains("scriptData")) {
                    jComp["type"] = "Script";
                    jComp["scripts"] = json::array();
                    jComp["scripts"].push_back(jComp["scriptData"]);
                    jComp.erase("scriptData");
                } else {
                    jComp["type"] = info.name;
                }
                
                jComponents.push_back(jComp);
            }
        }
        jPrefab["components"] = jComponents;

        std::ofstream file(path);
        if (!file.is_open()) return false;
        file << jPrefab.dump(4);
        
        Engine::Console::Log(std::format("SceneLoader: Successfully saved prefab to {}", path));
        return true;
    } catch (const std::exception& e) {
        Engine::Console::LogError(std::format("SceneLoader: Error saving prefab {}: {}", path, e.what()));
        return false;
    }
}

bool SceneLoader::SaveScene(const std::string& path, Engine::ECS::Registry& registry) {
    Engine::Console::Log(std::format("SceneLoader: Saving scene to {}", path));

    json data;
    json jEntities = json::array();

    auto& compReg = Engine::ECS::ComponentRegistry::GetInstance();
    
    // 全ての有効なエンティティ（Tagを持っているもの）を走査
    auto& tagStorage = registry.GetStorage<Engine::ECS::Tag>();
    for (auto entity : tagStorage.GetEntities()) {
        json jEntity;
        jEntity["id"] = (int)entity;
        
        auto& tag = registry.GetComponent<Engine::ECS::Tag>(entity);
        jEntity["name"] = std::string(tag.name);
        jEntity["isActive"] = (tag.isActive != 0);

        // 親子関係の取得 (Transformがあれば)
        if (registry.HasComponent<Engine::ECS::Transform>(entity)) {
            auto& trans = registry.GetComponent<Engine::ECS::Transform>(entity);
            if (trans.parent != Engine::ECS::kNullEntity) {
                jEntity["parent"] = (int)trans.parent;
            } else {
                jEntity["parent"] = nullptr;
            }
        } else {
            jEntity["parent"] = nullptr;
        }

        json jComponents = json::array();
        for (auto& [typeId, info] : compReg.GetAll()) {
            // Tagは既に直下にシリアライズ済みなのでコンポーネントリストからは除外
            if (typeId == 100) continue;

            if (registry.HasComponent(entity, typeId)) {
                json jComp = compReg.SerializeComponent(registry, entity, typeId);
                
                // ScriptComponent の場合は特別な構造にする
                if (typeId == 3 && jComp.contains("scriptData")) {
                    jComp["type"] = "Script"; // 下位互換性/ロードロジックのため
                    jComp["scripts"] = json::array();
                    jComp["scripts"].push_back(jComp["scriptData"]);
                    jComp.erase("scriptData");
                } else {
                    jComp["type"] = info.name;
                }
                
                jComponents.push_back(jComp);
            }
        }
        jEntity["components"] = jComponents;
        jEntities.push_back(jEntity);
    }

    data["entities"] = jEntities;

    std::ofstream file(path);
    if (!file.is_open()) {
        Engine::Console::LogError(std::format("SceneLoader: Failed to open file for saving: {}", path));
        return false;
    }

    file << data.dump(4);
    Engine::Console::Log(std::format("SceneLoader: Successfully saved {} entities to {}.", jEntities.size(), path));
    return true;
}

} // namespace Engine::Scene
