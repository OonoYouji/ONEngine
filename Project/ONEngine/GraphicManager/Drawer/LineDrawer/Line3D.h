#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>
#include <vector>
#include <memory>

#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include "GraphicManager/PipelineState/PipelineState.h"

using namespace Microsoft::WRL;


/// ===================================================
/// 三次元空間上の線を引くクラス
/// ===================================================
class Line3D final {

	/// ===================================================
	/// private : sub class
	/// ===================================================

	struct VertexData final {
		Vec4 position;
		Vec4 color;
	};


public:

	/// ===================================================
	/// public : static methods
	/// ===================================================

	static Line3D* GetInstance() {
		static Line3D instance;
		return &instance;
	}

	static void SInitialize(ID3D12GraphicsCommandList* commandList);
	static void SFinalize();



	/// ===================================================
	/// public : methods
	/// ===================================================

	Line3D() {}
	~Line3D() {}

	/// <summary>
	/// 線の描画
	/// </summary>
	/// <param name="v1">:		頂点1、ワールド座標	</param>
	/// <param name="v2">:		頂点2、ワールド座標	</param>
	/// <param name="v1Color">:	頂点1の色				</param>
	/// <param name="v2Color">:	頂点2の色				</param>
	void Draw(
		const Vec3& v1,
		const Vec3& v2, 
		const Vec4& v1Color = Vec4::kWhite, 
		const Vec4& v2Color = Vec4::kWhite
	);


	/// <summary>
	/// 描画前の処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後の処理
	/// </summary>
	void PostDraw();


private:

	/// ===================================================
	/// private : const objects
	/// ===================================================

	const uint32_t kMaxLineNum_ = 1028u;


	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<Line3D::VertexData> vertices_;

	ComPtr<ID3D12Resource>          vertexBuffer_  = nullptr;
	D3D12_VERTEX_BUFFER_VIEW        vbv_;

	std::unique_ptr<PipelineState>  pipelineState_ = nullptr;
	PipelineState::Shader           shader_;

	ID3D12GraphicsCommandList*      pCommandList_  = nullptr;

private:
	/// ===================================================
	/// private : copy constructors deleted
	/// ===================================================
	
	Line3D(const Line3D&)            = delete;
	Line3D(Line3D&&)                 = delete;
	Line3D& operator=(const Line3D&) = delete;
	Line3D& operator=(Line3D&&)      = delete;
};