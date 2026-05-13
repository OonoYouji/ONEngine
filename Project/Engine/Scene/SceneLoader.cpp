#include "SceneLoader.h"
#include "Externals/nlohmann/json.hpp"
#include "Engine/Common/Console.h"
#include "Engine/ECS/ComponentRegistry.h"
#include "Engine/Script/ScriptHost.h"
#include "Schema/Schema.h"
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

                if (jEntity.contains("parent") && !jEntity["parent"].is_null()) {
                    parentTasks.push_back({entity, jEntity["parent"]});
                }

                auto& componentRegistry = Engine::ECS::ComponentRegistry::GetInstance();
                if (jEntity.contains("components") && jEntity["components"].is_array()) {
                    for (const auto& jComp : jEntity["components"]) {
                        std::string type = jComp.value("type", "");
                        const auto* info = componentRegistry.GetInfo(type);
                        if (info) {
                            Engine::Console::Log(std::format("  {}: Loading component '{}'", entityLogName, type));
                            info->deserializeFunc(jComp, entity, registry);
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
        return true;
    } catch (const std::exception& e) {
        Engine::Console::LogError(std::format("SceneLoader Error: {}", e.what()));
        return false;
    }
}

Engine::ECS::Entity SceneLoader::InstantiatePrefab(const std::string& path, Engine::ECS::Registry& registry) {
    return Engine::ECS::kNullEntity; // TODO
}

} // namespace Engine::Scene
