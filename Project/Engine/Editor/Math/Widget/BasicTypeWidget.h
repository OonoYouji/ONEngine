#pragma once

/// std
#include <string>

/// external
#include <imgui.h>

namespace Editor {
// ==================================================================================
// Int Wrappers
// ==================================================================================

/// @brief ImGui::DragInt のラッパー
bool DragInt(const std::string& _label, int& _v, float _v_speed = 1.0f, int _v_min = 0, int _v_max = 0, const char* _format = "%d", ImGuiSliderFlags _flags = 0, float _columnWidth = 100.0f);

/// @brief ImGui::SliderInt のラッパー
bool SliderInt(const std::string& _label, int& _v, int _v_min, int _v_max, const char* _format = "%d", ImGuiSliderFlags _flags = 0, float _columnWidth = 100.0f);

/// @brief ImGui::InputInt のラッパー
bool InputInt(const std::string& _label, int& _v, int _step = 1, int _step_fast = 100, ImGuiInputTextFlags _flags = 0, float _columnWidth = 100.0f);

// ==================================================================================
// Float Wrappers
// ==================================================================================

/// @brief ImGui::DragFloat のラッパー
bool DragFloat(const std::string& _label, float& _v, float _v_speed = 1.0f, float _v_min = 0.0f, float _v_max = 0.0f, const char* _format = "%.3f", ImGuiSliderFlags _flags = 0, float _columnWidth = 100.0f);

/// @brief ImGui::SliderFloat のラッパー
bool SliderFloat(const std::string& _label, float& _v, float _v_min, float _v_max, const char* _format = "%.3f", ImGuiSliderFlags _flags = 0, float _columnWidth = 100.0f);

/// @brief ImGui::InputFloat のラッパー
bool InputFloat(const std::string& _label, float& _v, float _step = 0.0f, float _step_fast = 0.0f, const char* _format = "%.3f", ImGuiInputTextFlags _flags = 0, float _columnWidth = 100.0f);

} /// namespace Editor