#pragma once

#include "MetaReflectionUI.h"
#include "../../../Asset/Assets/Texture/Texture.h"
#include "../../../Asset/Assets/AudioClip/AudioClip.h"
#include "../../../Asset/Assets/Material/Material.h"
#include "../../../Asset/Assets/Mesh/Model.h"
#include "../../../Asset/Assets/Shader/Shader.h"


namespace Editor {


///
/// Texture
///
template<>
struct MetaReflection<ONEngine::Asset::Texture::MetaData> {
	static auto GetFields() {
		using T = ONEngine::Asset::Texture::MetaData;
		return std::make_tuple(
			MakeField("sRGB", &T::format),
			MakeField("Mipmap", &T::colorSpace)
		);
	}
};


///
/// Model
///
template<>
struct MetaReflection<ONEngine::Asset::Model::MetaData> {
	static auto GetFields() {
		using T = ONEngine::Asset::Model::MetaData;
		return std::make_tuple(
			MakeField("Scale", &T::scale)
		);
	}
};


///
/// Material
///
template<>
struct MetaReflection<ONEngine::Asset::Material::MetaData> {
	static auto GetFields() {
		using T = ONEngine::Asset::Material::MetaData;
		return std::make_tuple(
			MakeField("UseShader", &T::useShader),
			MakeField("AlbedoColor", &T::albedoColor),
			MakeField("AlbedoTexture", &T::albedoTextureGuid),
			MakeField("NormalTexture", &T::normalTextureGuid)
		);
	}
};


///
/// Shader
///
template<>
struct MetaReflection<ONEngine::Asset::Shader::MetaData> {
	static auto GetFields() {
		using T = ONEngine::Asset::Shader::MetaData;
		return std::make_tuple(
			MakeField("EntryPoint", &T::entryPoint),
			MakeField("Profile", &T::profile),
			MakeField("ShaderStage", &T::stage)
		);
	}
};


///
/// AudioClip
///
template<>
struct MetaReflection<ONEngine::Asset::AudioClip::MetaData> {
	static auto GetFields() {
		using T = ONEngine::Asset::AudioClip::MetaData;
		return std::make_tuple(
			MakeField("duration", &T::duration)
		);
	}
};


} /// namespace Editor