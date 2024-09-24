#include <Sprite.h>

#include <algorithm>

#include <WinApp.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxResourceCreator.h>

#include <SpriteManager.h>
#include <TextureManager.h>

#include <Component/Transform/Transform.h>


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

	material_.CreateBuffer();

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

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);

	pTransform_->BindTransform(commandList, 1);
	material_.BindMaterial(commandList, 2);
	material_.BindTexture(commandList, 3);
}

void Sprite::SetTexture(const std::string& textureName, const std::string& filePath) {
	TextureManager::GetInstance()->Load(textureName, filePath);

	material_.SetTextureName(textureName);
	material_.SetFilePath(filePath);
}

void Sprite::SetAnchor(const Vec2& anchor) {
	anchor_ = anchor;

	std::vector<VertexPosUv> vertices = vertices_;

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





