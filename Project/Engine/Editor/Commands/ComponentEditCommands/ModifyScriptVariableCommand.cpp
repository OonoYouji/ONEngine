#include "ModifyScriptVariableCommand.h"

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/Core/Utility/Utility.h"

/// external
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace Editor {

/**
 * @brief コンストラクタ
 */
ModifyScriptVariableCommand::ModifyScriptVariableCommand(ONEngine::GameEntity* _entity, const std::string& _scriptName, const std::string& _fieldName, int _monoType, const VariantValue& _oldValue, const VariantValue& _newValue)
    : entityGuid_(_entity->GetGuid()), scriptName_(_scriptName), fieldName_(_fieldName), monoType_(_monoType), oldValue_(_oldValue), newValue_(_newValue) {
}

/**
 * @brief コマンドの実行（新しい値を変数へ設定します）。
 */
EDITOR_STATE ModifyScriptVariableCommand::Execute() {
    ApplyValue(newValue_);
    return EDITOR_STATE_FINISH;
}

/**
 * @brief コマンドの取り消し処理（元の古い値を変数へ再設定します）。
 */
EDITOR_STATE ModifyScriptVariableCommand::Undo() {
    ApplyValue(oldValue_);
    return EDITOR_STATE_FINISH;
}

/**
 * @brief C# アセンブリを介し、対象フィールドへバリアント値を適切にキャストして書き込みます。
 */
void ModifyScriptVariableCommand::ApplyValue(const VariantValue& _value) {
    auto& monoEngine = ONEngine::MonoScriptEngine::GetInstance();
    
    ONEngine::GameEntity* entity = monoEngine.GetOwnerEntity(entityGuid_);
    if (!entity) {
        ONEngine::Console::LogError(std::format("[ModifyScriptVariableCommand] FAILED: Entity not found for GUID: {}", entityGuid_.ToString()));
        return;
    }

    std::string groupName = monoEngine.GetGroupNameByEntityGuid(entityGuid_);
    if (groupName.empty()) groupName = "GameScene";

    // 1. C#側のインスタンスがあれば更新を試みる
    bool appliedToMono = false;
    MonoObject* obj = monoEngine.GetMonoBehaviorFromCS(groupName, entity->GetId(), scriptName_);
    if (obj) {
        MonoClass* monoClass = mono_object_get_class(obj);
        MonoClassField* field = mono_class_get_field_from_name(monoClass, fieldName_.c_str());
        if (field) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    MonoString* ms = mono_string_new(mono_domain_get(), arg.c_str());
                    mono_field_set_value(obj, field, ms);
                } else {
                    mono_field_set_value(obj, field, (void*)&arg);
                }
                appliedToMono = true;
            }, _value);
        }
    }

    // 2. Variablesコンポーネント（永続化データ）を更新
    // これにより、起動前でもUndo/Redoが効き、次回の起動時に反映される
    ONEngine::Variables* vars = entity->GetComponent<ONEngine::Variables>();
    if (!vars) {
        vars = entity->AddComponent<ONEngine::Variables>();
    }

    if (vars) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            ONEngine::Variables::Var v;
            if constexpr (std::is_same_v<T, double>) {
                v = static_cast<float>(arg);
            } else {
                v = arg;
            }
            
            vars->SetVariable(scriptName_, fieldName_, v);
        }, _value);
        
        if (!appliedToMono) {
            ONEngine::Console::Log(std::format("[UndoDebug] SUCCESS: Applied to Variables component for '{}' (Mono instance not ready)", fieldName_));
        }
    }

    if (appliedToMono) {
        ONEngine::Console::Log(std::format("[ModifyScriptVariableCommand] SUCCESS: Applied to Mono instance for '{}' on '{}'", fieldName_, entity->GetName()));
    }
}

} /// Editor
