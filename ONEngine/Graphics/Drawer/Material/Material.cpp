#include <Material.h>


Material::Material() {}
Material::~Material() {}


/// ===================================================
/// テクスチャの名前をセット
/// ===================================================
void Material::SetTextureName(const std::string& filePath) {
	filePath_ = filePath;
}
