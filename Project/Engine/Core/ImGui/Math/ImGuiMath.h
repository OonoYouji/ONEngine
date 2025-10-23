#pragma once

/// std
#include <string>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Graphics/Buffer/Data/GPUMaterial.h"

/// 前方宣言
class AssetCollection;

/// ////////////////////////////////////////////////////////
/// ImGui用のMath関数群
/// ////////////////////////////////////////////////////////
namespace ImMathf {

	/// @brief Vector4 -> ImVec4 変換
	/// @param _vec 自作のVector4
	/// @return 変換されたImVec4
	ImVec4 ToImVec4(const Vector4& _vec);

	/// @brief Vector2 -> ImVec2 変換
	/// @param _vec 自作のVector2
	/// @return 変換されたImVec2
	ImVec2 ToImVec2(const Vector2& _vec);

	/// 色の編集
	bool ColorEdit(const char* _label, Vector4* _color, ImGuiColorEditFlags _flags = 0);

	/// テキストの入力
	bool InputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags = 0);

	/// マテリアルの編集
	bool MaterialEdit(const char* _label, GPUMaterial* _material, AssetCollection* _assetCollection);

	/// UV変形の編集
	bool UVTransformEdit(const char* _label, UVTransform* _uvTransform);

}


/// -----------------------------------------------
/// まだImMathfに移動していない関数
/// -----------------------------------------------

bool ImGuiInputText(const char* _label, std::string* _text, ImGuiInputTextFlags _flags = 0);

void ImGuiInputTextReadOnly(const char* _label, const std::string& _text);

bool ImGuiColorEdit(const char* _label, Vector4* _color);


/// //////////////////////////////////////////////
/// componentのデバッグ表示関数を定義 (今後各Componentの.h .cppに移動予定)
/// //////////////////////////////////////////////

void DirectionalLightDebug(class DirectionalLight* _light);

void AudioSourceDebug(class AudioSource* _audioSource);

void MeshRendererDebug(class MeshRenderer* _meshRenderer);

void CustomMeshRendererDebug(class CustomMeshRenderer* _customMeshRenderer);

void EffectDebug(class Effect* _effect);

