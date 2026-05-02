#pragma once

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include <string>

namespace Editor {

// ==================================================================================
// Floating Point Vectors
// ==================================================================================

/// @brief ImGuiを用いて Vector2 (float) を編集するGUIウィジェットを描画します。
/// @param _min 最小値 (0かつmaxも0の場合は制限なし)
/// @param _max 最大値 (0かつminも0の場合は制限なし)
bool DrawVec2Control(const std::string& _label, ONEngine::Vector2& _values, float _speed = 1.0f, float _min = 0.0f, float _max = 0.0f, float _columnWidth = 100.0f, bool* _unified = nullptr);

bool DrawVec3Control(const std::string& _label, ONEngine::Vector3& _values, float _speed = 1.0f, float _min = 0.0f, float _max = 0.0f, float _columnWidth = 100.0f, bool* _unified = nullptr);

bool DrawVec4Control(const std::string& _label, ONEngine::Vector4& _values, float _speed = 1.0f, float _min = 0.0f, float _max = 0.0f, float _columnWidth = 100.0f, bool* _unified = nullptr);

// ==================================================================================
// Integer Vectors
// ==================================================================================

/// @brief ImGuiを用いて Vector2Int (int) を編集するGUIウィジェットを描画します。
/// @param _min 最小値 (0かつmaxも0の場合は制限なし)
/// @param _max 最大値 (0かつminも0の場合は制限なし)
bool DrawVec2IntControl(const std::string& _label, ONEngine::Vector2Int& _values, float _speed = 1.0f, int _min = 0, int _max = 0, float _columnWidth = 100.0f);

bool DrawVec3IntControl(const std::string& _label, ONEngine::Vector3Int& _values, float _speed = 1.0f, int _min = 0, int _max = 0, float _columnWidth = 100.0f);

bool DrawVec4IntControl(const std::string& _label, ONEngine::Vector4Int& _values, float _speed = 1.0f, int _min = 0, int _max = 0, float _columnWidth = 100.0f);

} /// namespace Editor