#include "Sprite.h"

#include <algorithm>

#include "WindowManager/WinApp.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "SpriteManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"

#include "ComponentManager/Transform/Transform.h"


Sprite::Sprite() {}
Sprite::~Sprite() {}


/// ===================================================
/// 初期化
/// ===================================================
void Sprite::Initialize(const std::string& textureName, const std::string& filePath) {

	/// ---------------------------------------------------
	/// メッシュの初期化
	/// ---------------------------------------------------
	CreateVertexBuffer();


	/// ---------------------------------------------------
	/// マテリアルの初期化
	/// ---------------------------------------------------
	SetTexture(textureName, filePath);

	TextureManager::GetInstance()->GetTexture(textureName);

	textureSize_ = TextureManager::GetInstance()->GetTexture(textureName).GetTextureSize();

	material_.CreateBuffer();

}



/// ===================================================
/// 描画
/// ===================================================
void Sprite::Draw() {
	SpriteManager::GetInstance()->AddActiveSprite(this);
}



/// ===================================================
/// commandListに情報を設定
/// ===================================================
void Sprite::BindCBuffer(ID3D12GraphicsCommandList* commandList) {

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);

	pTransform_->BindTransform(commandList, 1);
	material_.BindMaterial(commandList, 2);
	material_.BindTexture(commandList, 3);
}




void Sprite::SetColor(const Vec4& color) {
	material_.SetColor(color);
}

void Sprite::SetTexture(const std::string& textureName, const std::string& filePath, bool isDefaultScaling) {
	TextureManager::GetInstance()->Load(textureName, filePath);

	material_.SetTextureName(textureName);
	material_.SetFilePath(filePath);

	textureSize_ = TextureManager::GetInstance()->GetTexture(textureName).GetTextureSize();

	if(isDefaultScaling) {
		SetUVSize(textureSize_);
	}
}

void Sprite::SetAnchor(const Vec2& anchor) {
	anchor_ = anchor;

	/// 左下
	vertices_[0].position = {
		std::lerp( 0.0f, -1.0f, anchor_.x),
		std::lerp(-1.0f,  0.0f, anchor_.y),
		0.0f, 1.0f
	};

	/// 左上
	vertices_[1].position = {
		std::lerp( 0.0f, -1.0f, anchor_.x),
		std::lerp( 0.0f,  1.0f, anchor_.y),
		0.0f, 1.0f
	};

	/// 右上
	vertices_[2].position = {
		std::lerp( 1.0f,  0.0f, anchor_.x),
		std::lerp( 0.0f,  1.0f, anchor_.y),
		0.0f, 1.0f
	};

	/// 右下
	vertices_[3].position = {
		std::lerp( 1.0f, 0.0f, anchor_.x),
		std::lerp(-1.0f, 0.0f, anchor_.y),
		0.0f, 1.0f
	};

	VertexPosUv* vertexData = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexPosUv) * vertices_.size());

}

void Sprite::SetTransformToPointer(Transform* transform) {
	pTransform_ = transform;
}



void Sprite::SetUVSize(const Vec2& size) {
	uvSize_ = size;
	material_.SetScale(uvSize_ / textureSize_);
}

void Sprite::SetUVRotate(float rotate) {
	rotate_ = rotate;
	material_.SetRotate(rotate_);
}

void Sprite::SetUVPosition(const Vec2& position) {
	uvPosition_ = position;
	material_.SetPosition(uvPosition_ / textureSize_);
}

void Sprite::UpdateMatrix() {
	material_.UpdateMatrix();
}




/// ===================================================
/// 頂点バッファの生成
/// ===================================================
void Sprite::CreateVertexBuffer() {

	vertices_.push_back({ { -0.5f, -0.5f, 0.0f, 1.0f}, { 0.0f, 1.0f } }); ///-  左下
	vertices_.push_back({ { -0.5f,  0.5f, 0.0f, 1.0f}, { 0.0f, 0.0f } }); ///-  左上
	vertices_.push_back({ {  0.5f,  0.5f, 0.0f, 1.0f}, { 1.0f, 0.0f } }); ///-  右上
	vertices_.push_back({ {  0.5f, -0.5f, 0.0f, 1.0f}, { 1.0f, 1.0f } }); ///-  右下

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





