#pragma once

/// std
#include <string>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"


bool ImGuiInputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags = ImGuiInputTextFlags_CallbackCharFilter);

void ImGuiInputTextReadOnly(const char* _label, const std::string& _text);

bool ImGuiColorEdit(const char* _label, Vector4* _color);


/// //////////////////////////////////////////////
/// componentのデバッグ表示関数を定義
/// //////////////////////////////////////////////

/// @brief tranfsorm のデバッグ表示
/// @param _transform 
void TransformDebug(class Transform* _transform);

void DirectionalLightDebug(class DirectionalLight* _light);

void AudioSourceDebug(class AudioSource* _audioSource);

void VariablesDebug(class Variables* _variables);

void MeshRendererDebug(class MeshRenderer* _meshRenderer);

void CustomMeshRendererDebug(class CustomMeshRenderer* _customMeshRenderer);

void EffectDebug(class Effect* _effect);

void ValueImGui(class Variables* _variables, const std::string& _label, const std::string& _name, size_t _type);
