#pragma once

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"

namespace Editor {

namespace ImMathf {

// ==================================================================================
// Floating Point Vectors
// ==================================================================================

/// @brief ImGuiを用いて Vector2 (float) を編集するGUIウィジェットを描画します。
/// @param _label ラベル名（IDとしても使用されます）
/// @param _values 編集対象のVector2変数の参照
/// @param _speed ドラッグ時の数値変化速度
/// @param _columnWidth ラベル表示部分のカラム幅
/// @param _unified 比率固定（Uniform Scale）の有効状態を管理するフラグへのポインタ。nullptrの場合は機能が無効化されます。
/// @return 値が変更された場合に true を返します。
bool DrawVec2Control(const std::string& _label, ONEngine::Vector2& _values, float _speed, float _columnWidth, bool* _unified);

/// @brief ImGuiを用いて Vector3 (float) を編集するGUIウィジェットを描画します。
/// @param _label ラベル名（IDとしても使用されます）
/// @param _values 編集対象のVector3変数の参照
/// @param _speed ドラッグ時の数値変化速度
/// @param _columnWidth ラベル表示部分のカラム幅
/// @param _unified 比率固定（Uniform Scale）の有効状態を管理するフラグへのポインタ。nullptrの場合は機能が無効化されます。
/// @return 値が変更された場合に true を返します。
bool DrawVec3Control(const std::string& _label, ONEngine::Vector3& _values, float _speed, float _columnWidth, bool* _unified);

/// @brief ImGuiを用いて Vector4 (float) を編集するGUIウィジェットを描画します。
/// @param _label ラベル名（IDとしても使用されます）
/// @param _values 編集対象のVector4変数の参照
/// @param _speed ドラッグ時の数値変化速度
/// @param _columnWidth ラベル表示部分のカラム幅
/// @param _unified 比率固定（Uniform Scale）の有効状態を管理するフラグへのポインタ。nullptrの場合は機能が無効化されます。
/// @return 値が変更された場合に true を返します。
bool DrawVec4Control(const std::string& _label, ONEngine::Vector4& _values, float _speed, float _columnWidth, bool* _unified);

// ==================================================================================
// Integer Vectors
// ==================================================================================

/// @brief ImGuiを用いて Vector2Int (int) を編集するGUIウィジェットを描画します。
/// @param _label ラベル名（IDとしても使用されます）
/// @param _values 編集対象のVector2Int変数の参照
/// @param _speed ドラッグ時の数値変化速度（整数値として加算されます）
/// @param _columnWidth ラベル表示部分のカラム幅
/// @return 値が変更された場合に true を返します。
bool DrawVec2IntControl(const std::string& _label, ONEngine::Vector2Int& _values, float _speed, float _columnWidth);

/// @brief ImGuiを用いて Vector3Int (int) を編集するGUIウィジェットを描画します。
/// @param _label ラベル名（IDとしても使用されます）
/// @param _values 編集対象のVector3Int変数の参照
/// @param _speed ドラッグ時の数値変化速度（整数値として加算されます）
/// @param _columnWidth ラベル表示部分のカラム幅
/// @return 値が変更された場合に true を返します。
bool DrawVec3IntControl(const std::string& _label, ONEngine::Vector3Int& _values, float _speed, float _columnWidth);

/// @brief ImGuiを用いて Vector4Int (int) を編集するGUIウィジェットを描画します。
/// @param _label ラベル名（IDとしても使用されます）
/// @param _values 編集対象のVector4Int変数の参照
/// @param _speed ドラッグ時の数値変化速度（整数値として加算されます）
/// @param _columnWidth ラベル表示部分のカラム幅
/// @return 値が変更された場合に true を返します。
bool DrawVec4IntControl(const std::string& _label, ONEngine::Vector4Int& _values, float _speed, float _columnWidth);

} /// namespace ImMathf
} /// namespace Editor