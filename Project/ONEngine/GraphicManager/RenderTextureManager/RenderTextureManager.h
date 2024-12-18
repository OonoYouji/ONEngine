#pragma once

#include <unordered_map>

#include "GraphicManager/RenderTextureManager/RenderTexture.h"


class RenderTextureManager final {
	RenderTextureManager() {}
	~RenderTextureManager() {}
public:

	static RenderTextureManager* GetInstance() {
		return &sInstance_;
	}

	/// <summary>
	/// render targetの設定
	/// </summary>
	/// <param name="name"> : renderTextures_のkey</param>
	static void BeginRenderTarget(const std::string& name);
	static void EndRenderTarget(const std::string& name);

	//static void CreateRenderTarget(const std::string& name, uint32_t layerNumber, const Vector4& clearColor);
	static void SetIsBlending(const std::string& name, bool isBlending);
	static RenderTexture* GetRenderTexture(const std::string& name);

	/// <summary>
	/// thisの初期化
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="descriptor"></param>
	void Initialize(ID3D12GraphicsCommandList* commandList, ONE::DxCommon* _dxCommon);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	void ImGuiDebug();

	/// <summary>
	/// フレームの最後に行う
	/// </summary>
	void EndFrame();

	/// <summary>
	/// コマンドリストにバインド
	/// </summary>
	void BindForCommandList();

	RenderTexture* GetFinalRenderTexture() const {
		return finalRenderTex_.get();
	}

	static void CreateBlendRenderTexture(
		std::vector<RenderTexture*> renderTextures,
		RenderTexture* outputRenderTexture
	);

private:

	static RenderTextureManager sInstance_;

	std::vector<std::unique_ptr<RenderTexture>> renderTextures_;

	struct RenderTexData {
		uint32_t layerNum;
		bool isBlending;
		bool isRenderTargetActive;
	};
	std::unordered_map<std::string, RenderTexData> renderTexDatas_;

	std::unique_ptr<RenderTexture> finalRenderTex_;
	std::unique_ptr<RenderTexture> intermediateTextures_[2];
	int intermediateIndex_ = 0;



	std::unique_ptr<PipelineState> pipeline_;
	ComPtr<ID3D12Resource> viewProjectionBuffer_;
	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbv_;

	/// other class pointer
	ID3D12GraphicsCommandList* pCommandList_;

private:
	RenderTextureManager(const RenderTextureManager&) = delete;
	RenderTextureManager(RenderTextureManager&&) = delete;
	RenderTextureManager& operator= (const RenderTextureManager&) = delete;
	RenderTextureManager& operator= (RenderTextureManager&&) = delete;
};