#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <string>
#include <vector>

#include "GraphicManager/Drawer/Material/Material.h"
#include "GraphicManager/PipelineState/PipelineState.h"

#include "ComponentManager/Transform/Transform.h"

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
	void Draw();


	void BindCBuffer(ID3D12GraphicsCommandList* commandList);


	/// ===================================================
	/// public : methods
	/// ===================================================

	void SetColor(const Vec4& color);

	void SetTexture(const std::string& textureName, const std::string& filePath, bool isDefaultScaling = true);

	void SetAnchor(const Vec2& anchor);
	const Vec2& GetAnchor() const { return anchor_; }

	void SetTransformToPointer(Transform* transform);

	/// accessor : uv size
	void SetUVSize(const Vec2& size);
	const Vec2& GetUVSize() const { return uvSize_; }

	/// accessor : uv rotate
	void SetUVRotate(float rotate);
	float GetUVRotate() const { return rotate_; }

	/// accessor : uv position
	void SetUVPosition(const Vec2& position);
	const Vec2& GetUVPosition() const { return uvPosition_; }

	void UpdateMatrix();

private:


	/// ===================================================
	/// private : methods
	/// ===================================================

	void CreateVertexBuffer();


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

	class Transform* pTransform_ = nullptr;
	Material material_;

	Vec2 anchor_     = { 0.5f, 0.5f };

	Vec2  uvSize_     = { 1.0f, 1.0f };
	float rotate_     = 0.0f;
	Vec2  uvPosition_ = { 0.0f, 0.0f };

	Vec2 textureSize_ = {};

};