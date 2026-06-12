#pragma once

/// std
#include <string>
#include <variant>
#include <vector>

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "../IEditCommand.h"

namespace ONEngine { class GameEntity; }

namespace Editor {

/// ///////////////////////////////////////////////
/// スクリプト変数の編集用コマンド
/// ///////////////////////////////////////////////
class ModifyScriptVariableCommand : public IEditCommand {
public:
    using VariantValue = std::variant<int, float, double, bool, std::string, ONEngine::Vector2, ONEngine::Vector3, ONEngine::Vector4>;

    ModifyScriptVariableCommand(ONEngine::GameEntity* _entity, const std::string& _scriptName, const std::string& _fieldName, int _monoType, const VariantValue& _oldValue, const VariantValue& _newValue);
    ~ModifyScriptVariableCommand() override = default;

    EDITOR_STATE Execute() override;
    EDITOR_STATE Undo() override;

private:
    void ApplyValue(const VariantValue& _value);

    ONEngine::Guid entityGuid_;
    std::string scriptName_;
    std::string fieldName_;
    int monoType_;
    VariantValue oldValue_;
    VariantValue newValue_;
};

} /// Editor
