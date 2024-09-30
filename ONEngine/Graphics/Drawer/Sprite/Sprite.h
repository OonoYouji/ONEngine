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
#include "../../../Component/Transform/Transform.h"

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

	void SetTexture(const std::string& textureName, const std::string& filePath);

	void SetAnchor(const Vec2& anchor);
	const Vec2& GetAnchor() const { return anchor_; }

	void SetTransformToPointer(Transform* transform);

	void SetUVSize(const Vec2& size);
	void SetUVRotate(float rotate);
	void SetUVPosition(const Vec2& position);

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

	Vec2 anchor_ = { 0.5f, 0.5f };

};