#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <string>
#include <vector>

#include <Material.h>
#include <PipelineState.h>

#include <Vector2.h>
#include <Vector4.h>
#include <Matrix4x4.h>

#include <Model.h>

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
	/// static変数の初期化
	/// </summary>
	static void StaticInitialize();
	
	/// <summary>
	/// static変数の終了処理
	/// </summary>
	static void StaticFinalize();

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

private:

	void CreateVertexBuffer();

	void CreateConstantBuffer();

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================

	static std::unique_ptr<PipelineState> pipelineState_;
	static PipelineState::Shader shader_;

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
	TransformData* transformData_ = nullptr;
	
	ComPtr<ID3D12Resource> viewProjectionBuffer_ = nullptr;
	ViewProjectionData* viewProjectionData_ = nullptr;

	Material material_;

};