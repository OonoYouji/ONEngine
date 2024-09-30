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

void Material::SetIsLighting(bool isLighting) {
	materialData_->isLighting = isLighting;
}



void Material::SetPosition(const Vec2& position) {
	position_ = position;

	materialData_->uvTransform = Mat3::MakeAffine(scale_, rotate_, position_).ToMat4();
}

void Material::SetRotate(float rotate) {
	rotate_ = rotate;

	materialData_->uvTransform = Mat3::MakeAffine(scale_, rotate_, position_).ToMat4();
}

void Material::SetScale(const Vec2& scale) {
	scale_ = scale;

	materialData_->uvTransform = Mat3::MakeAffine(scale_, rotate_, position_).ToMat4();
}



/// ===================================================
/// バッファの生成
/// ===================================================
void Material::CreateBuffer() {
	if(materialBuffer_) { return; }
	materialBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Material));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vec4(1, 1, 1, 1);
	materialData_->uvTransform = Mat3::kIdentity.ToMat4();
	materialData_->isLighting = true;
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
