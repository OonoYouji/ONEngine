#pragma once

/// std
#include <string>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"


namespace ImMathf {

	ImVec4 ToImVec4(const Vector4& _vec);
	ImVec2 ToImVec2(const Vector2& _vec);

	bool InputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags = 0);
}


bool ImGuiInputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags = 0);

void ImGuiInputTextReadOnly(const char* _label, const std::string& _text);

bool ImGuiColorEdit(const char* _label, Vector4* _color);


/// //////////////////////////////////////////////
/// componentのデバッグ表示関数を定義
/// //////////////////////////////////////////////

void DirectionalLightDebug(class DirectionalLight* _light);

void AudioSourceDebug(class AudioSource* _audioSource);

void VariablesDebug(class Variables* _variables);

void MeshRendererDebug(class MeshRenderer* _meshRenderer);

void CustomMeshRendererDebug(class CustomMeshRenderer* _customMeshRenderer);

void EffectDebug(class Effect* _effect);

void ValueImGui(class Variables* _variables, const std::string& _label, const std::string& _name, size_t _type);
