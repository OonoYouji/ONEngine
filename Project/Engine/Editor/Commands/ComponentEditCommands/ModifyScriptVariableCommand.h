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
/**
 * @class ModifyScriptVariableCommand
 * @brief エディタのインスペクター上で C# スクリプトコンポーネント（MonoBehavior継承クラス）の各種パブリック変数を編集した際、Undo / Redo を可能にするコマンドクラス
 */
class ModifyScriptVariableCommand : public IEditCommand {
public:
    using VariantValue = std::variant<int, float, double, bool, std::string, ONEngine::Vector2, ONEngine::Vector3, ONEngine::Vector4>;

    /**
     * @brief コンストラクタ
     * @param _entity 編集対象のスクリプトを持つエンティティへのポインタ
     * @param _scriptName 変更対象スクリプトのクラス名（文字列）
     * @param _fieldName 変更対象の変数（フィールド）名
     * @param _monoType 対象フィールドの C# 側のデータ型を表す MonoType 列挙値
     * @param _oldValue 変更前の値（バリアント型）
     * @param _newValue 変更後の値（バリアント型）
     */
    ModifyScriptVariableCommand(ONEngine::GameEntity* _entity, const std::string& _scriptName, const std::string& _fieldName, int _monoType, const VariantValue& _oldValue, const VariantValue& _newValue);

    /**
     * @brief デストラクタ
     */
    ~ModifyScriptVariableCommand() override = default;

    /**
     * @brief コマンドの実行（新しい値を変数へ設定します）。
     * @return 実行結果ステート
     */
    EDITOR_STATE Execute() override;

    /**
     * @brief コマンドの取り消し処理（元の古い値を変数へ再設定します）。
     * @return 実行結果ステート
     */
    EDITOR_STATE Undo() override;

private:
    /**
     * @brief C# アセンブリを介し、対象フィールドへバリアント値を適切にキャストして書き込みます。
     * @param _value 書き込む値
     */
    void ApplyValue(const VariantValue& _value);

    ONEngine::Guid entityGuid_;
    std::string scriptName_;
    std::string fieldName_;
    int monoType_;
    VariantValue oldValue_;
    VariantValue newValue_;
};

} /// Editor
