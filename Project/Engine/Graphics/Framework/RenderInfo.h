#pragma once

/// std
#include <array>
#include <string>


namespace ONEngine {
namespace RenderInfo {

/*
*  [使用方法]
*  - MRTの一枚のテクスチャ欲しい
*    kRenderTargetDir + kRenderTargetNames[static_cast<size_t>(RenderTexture::<種類>)] + kRenderTargetType[static_cast<size_t>(RenderTextureType::<種類>)]
*/


/// @brief .metaが保存されているディレクトリ
inline const std::string kRenderTargetDir = "./Assets/Scene/RenderTexture/";


/// @brief RTVとして使うレンダーテクスチャの種類
enum class RenderTexture {
	Scene,
	Debug,
	Prefab,
	ShadowMap,
};

/// @brief 各レンダーテクスチャの名前
inline constexpr size_t kRenderTextureCount = 4;
inline const std::array<std::string, kRenderTextureCount> kRenderTargetNames = {
	"scene",
	"debug",
	"prefab",
	"shadowMap"
};


/// @brief MRT用レンダーテクスチャの種類
enum class RenderTextureType {
	Scene,
	WorldPosition,
	Normal,
	Flags,
};

/// @brief MRT用レンダーテクスチャの名前
inline constexpr size_t kRenderTextureTypeCount = 4;
inline const std::array<std::string, kRenderTextureTypeCount> kRenderTargetType = {
	"Scene",
	"WorldPosition",
	"Normal",
	"Flags"
};



} /// namespace RenderInfo
} /// namespace ONEngine