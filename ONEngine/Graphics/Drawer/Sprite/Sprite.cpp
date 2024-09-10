#include <Sprite.h>

#include <WinApp.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>

#include <SpriteManager.h>
#include <TextureManager.h>

#include <Vector3.h>


Sprite::Sprite() {}
Sprite::~Sprite() {}


/// ===================================================
/// 初期化
/// ===================================================
void Sprite::Initialize(const std::string& textureName, const std::string& filePath) {

	TextureManager::GetInstance()->Load(
		textureName, filePath);

	/// ---------------------------------------------------
	/// メッシュの初期化
	/// ---------------------------------------------------
	CreateVertexBuffer();


	/// ---------------------------------------------------
	/// マテリアルの初期化
	/// ---------------------------------------------------
	material_.SetFilePath(filePath);
	material_.SetTextureName(textureName);

	material_.CreateBuffer();


	/// ---------------------------------------------------
	/// 定数バッファの初期化
	/// ---------------------------------------------------
	CreateConstantBuffer();


}



/// ===================================================
/// 描画
/// ===================================================
void Sprite::Draw(uint32_t zOrder) {
	SpriteManager::GetInstance()->AddActiveSprite(this, zOrder);
}



/// ===================================================
/// commandListに情報を設定
/// ===================================================
void Sprite::BindCBuffer(ID3D12GraphicsCommandList* commandList) {
	
	*matTransformData_ =
		Mat4::MakeScale(Vec3(size_.x, size_.y, 1.0f)) *
		Mat4::MakeRotateZ(angle_) *
		Mat4::MakeTranslate(position_);

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);

	commandList->SetGraphicsRootConstantBufferView(1, transformBuffer_->GetGPUVirtualAddress());
	material_.BindMaterial(commandList, 2);
	material_.BindTexture(commandList, 3);
}


void Sprite::SetPos(const Vec3& pos) {
	position_ = pos;
}

void Sprite::SetSize(const Vec2& textureSize) {
	size_ = textureSize;
}




/// ===================================================
/// 頂点バッファの生成
/// ===================================================
void Sprite::CreateVertexBuffer() {

	vertices_.push_back({ { -1.0f,  1.0f, 0.0f, 1.0f}, { 0.0f, 1.0f } }); ///-  左下
	vertices_.push_back({ { -1.0f, -1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f } }); ///-  左上
	vertices_.push_back({ {  1.0f, -1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f } }); ///-  右上
	vertices_.push_back({ {  1.0f,  1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f } }); ///-  右下

	indices_.push_back(0);
	indices_.push_back(1);
	indices_.push_back(2);
	indices_.push_back(0);
	indices_.push_back(2);
	indices_.push_back(3);


	/// ---------------------------------------------------
	/// 頂点
	/// ---------------------------------------------------
	vertexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(VertexPosUv) * vertices_.size());

	VertexPosUv* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexPosUv) * vertices_.size());

	vbv_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexPosUv) * vertices_.size());
	vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexPosUv));


	/// ---------------------------------------------------
	/// インデックス
	/// ---------------------------------------------------

	indexBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(uint32_t) * indices_.size());

	uint32_t* indexData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());

	ibv_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibv_.Format = DXGI_FORMAT_R32_UINT;
	ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());

}


/// ===================================================
/// 定数バッファの生成
/// ===================================================
void Sprite::CreateConstantBuffer() {

	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));

	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matTransformData_));
	*matTransformData_ =
		Mat4::MakeScale(Vec3(size_.x, size_.x, 1.0f)) *
		Mat4::MakeRotateZ(angle_) *
		Mat4::MakeTranslate(position_);

}




