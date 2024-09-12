#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <string>
#include <vector>

#include <Material.h>
#include <PipelineState.h>

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>

using namespace Microsoft::WRL;

/// ===================================================
/// 2d textureの描画クラス
/// ===================================================
class Sprite final {

	/// ===================================================
	/// private : sub class
	/// ===================================================
	struct VertexPosUv final {
		Vec4 position;
		Vec2 texcoord;
	};

public:

	Sprite();
	~Sprite();

	/// ===================================================
	/// public : methods
	/// ===================================================



	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureName"></param>
	/// <param name="filePath"></param>
	void Initialize(const std::string& textureName, const std::string& filePath);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(uint32_t zOrder = 0u);


	void BindCBuffer(ID3D12GraphicsCommandList* commandList);

	void SetColor(const Vec4& color) {
		material_.SetColor(color);
	}

	void SetPos(const Vec3& pos);

	void SetSize(const Vec2& textureSize);

	void SetAngle(float angle) { angle_ = angle; }

	void SetTexture(const std::string& textureName, const std::string& filePath);

private:


	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateVertexBuffer();

	void CreateConstantBuffer();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	ComPtr<ID3D12Resource> indexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbv_{};
	D3D12_INDEX_BUFFER_VIEW ibv_{};

	std::vector<VertexPosUv> vertices_;
	std::vector<uint32_t> indices_;


	ComPtr<ID3D12Resource> transformBuffer_ = nullptr;
	Mat4* matTransformData_ = nullptr;


	Material material_;

	Vec3 position_ = { 0,0,0 };
	Vec2 size_ = { 100,100 };
	float angle_ = 0.0f;

};