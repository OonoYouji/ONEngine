#pragma once

/// external
#include <imgui.h>

/// //////////////////////////////////////////////
/// componentのデバッグ表示関数を定義
/// //////////////////////////////////////////////

/// @brief tranfsorm のデバッグ表示
/// @param _transform 
void TransformDebug(class Transform* _transform);

void DirectionalLightDebug(class DirectionalLight* _light);