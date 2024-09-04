#include <Material.h>

#include <DxResourceCreator.h>

#include <TextureManager.h>



Material::Material() {}
Material::~Material() {}

void Material::CreateMaterial(const std::string& textureName) {
	CreateBuffer();
	SetTextureName(textureName);
}

void Material::SetColor(const Vector4& color) {
	materialData_->color = color;
}


/// ===================================================
/// バッファの生成
/// ===================================================
void Material::CreateBuffer() {
	materialBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Material));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vec4(1, 1, 1, 1);
	materialData_->uvTransform = Mat3::kIdentity.ToMat4();
}


/// ===================================================
/// テクスチャのファイルパスをセット
/// ===================================================
void Material::SetFilePath(const std::string& filePath) {
	filePath_ = filePath;
}


/// ===================================================
/// テクスチャの名前をセット
/// ===================================================
void Material::SetTextureName(const std::string& textureName) {
	texName_ = textureName;
}


/// ===================================================
/// テクスチャのバインド
/// ===================================================
void Material::BindTexture(ID3D12GraphicsCommandList* commandList, uint32_t rootParamIndex) {
	commandList->SetGraphicsRootDescriptorTable(rootParamIndex, TextureManager::GetInstance()->GetTexture(texName_).GetGPUHandle());
}


/// ===================================================
/// マテリアルのバインド
/// ===================================================
void Material::BindMaterial(ID3D12GraphicsCommandList* commandList, uint32_t rootParamIndex) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndex, materialBuffer_->GetGPUVirtualAddress());
}
