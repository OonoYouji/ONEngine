#include "ShaderLoader.h"

namespace ONEngine {

AssetLoader<Shader>::AssetLoader() {}

std::optional<Shader> AssetLoader<Shader>::Load(const std::string& filepath) {
	/// ----- Shaderの読み込み処理 ----- ///
	return std::nullopt;
}

std::optional<Shader> AssetLoader<Shader>::Reload(const std::string& filepath, Shader* src) {
	/// ----- Shaderの再読み込み処理 ----- ///
	return std::nullopt;
}


} /// namespace ONEngine