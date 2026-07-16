#pragma once

/// std
#include <string>
#include <vector>

/// external
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

/// engine
#include "Engine/Core/Utility/Math/Vector3.h"
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

namespace ONEngine::Asset {
class AssetCollection;
}

/// 前方宣言
namespace Editor {

/// ////////////////////////////////////////////////////////
/// ImGui用のMath関数群
/// ////////////////////////////////////////////////////////
namespace ImMathf {

/// @brief Vector4 -> ImVec4 変換
/// @param _vec 自作のVector4
/// @return 変換されたImVec4
ImVec4 ToImVec4(const ONEngine::Vector4& _vec);

/// @brief Vector2 -> ImVec2 変換
/// @param _vec 自作のVector2
/// @return 変換されたImVec2
ImVec2 ToImVec2(const ONEngine::Vector2& _vec);

/// 色の編集
bool ColorEdit(const char* _label, ONEngine::Vector4* _color, ImGuiColorEditFlags _flags = 0);

/// テキストの入力
bool InputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags = 0);

/// 数値の入力
bool InputFloat(const char* _label, float* _v, float _step = 0.0f, float _step_fast = 0.0f, const char* _format = "%.3f", ImGuiInputTextFlags _flags = 0);

/// 数値のドラッグ入力
bool DragFloat(const char* _label, float* _v, float _speed = 0.1f, float _min = 0.0f, float _max = 0.0f, const char* _format = "%.3f", ImGuiInputTextFlags _flags = 0);

/// Vector3のドラッグ入力
bool DragFloat3(const char* _label, ONEngine::Vector3* _v, float _speed = 0.1f, float _min = 0.0f, float _max = 0.0f, const char* _format = "%.3f", ImGuiInputTextFlags _flags = 0);

/// Enumの入力
template <typename T>
bool InputEnum(const char* _label, T* _value) {
    auto names = magic_enum::enum_names<T>();
    int current_item = static_cast<int>(magic_enum::enum_index(*_value).value_or(0));
    
    std::vector<const char*> item_ptrs;
    for (const auto& name : names) {
        item_ptrs.push_back(name.data());
    }
    
    if (ImGui::Combo(_label, &current_item, item_ptrs.data(), static_cast<int>(item_ptrs.size()))) {
        *_value = magic_enum::enum_value<T>(current_item);
        return true;
    }
    return false;
}

/// マテリアルの編集
bool MaterialEdit(const char* _label, ONEngine::GPUMaterial* _material, ONEngine::Asset::AssetCollection* _assetCollection);

/// UV変形の編集
bool UVTransformEdit(const char* _label, ONEngine::UVTransform* _uvTransform);


ImVec2 CalculateAspectFitSize(const ONEngine::Vector2& _textureSize, float _maxSize);
ImVec2 CalculateAspectFitSize(const ONEngine::Vector2& _textureSize, const ImVec2& _maxSize);
} /// ImMathf

/// -----------------------------------------------
/// まだImMathfに移動していない関数
/// -----------------------------------------------

bool ImGuiInputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags = 0);
bool ImGuiInputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags, const char* _hint);

void ImGuiInputTextReadOnly(const char* _label, const std::string& _text);

bool ImGuiColorEdit(const char* _label, ONEngine::Vector4* _color);


} /// Editor


namespace ONEngine {
/// //////////////////////////////////////////////
/// componentのデバッグ表示関数を定義 (今後各Componentの.h .cppに移動予定)
/// //////////////////////////////////////////////

void DirectionalLightDebug(class DirectionalLight* _light);

void AudioSourceDebug(class AudioSource* _audioSource);

void CustomMeshRendererDebug(class CustomMeshRenderer* _customMeshRenderer);

void EffectDebug(class Effect* _effect);
void ParticleSystemDebug(class ParticleSystem* _ps);

/// Unity-like modular header with checkbox
bool BeginModuleHeader(const char* label, bool* enabled);
void EndModuleHeader();

} /// ONEngine