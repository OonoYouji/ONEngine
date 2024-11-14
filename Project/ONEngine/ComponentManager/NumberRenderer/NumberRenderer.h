#pragma once

/// directX12
#include <wrl/client.h>
#include <d3d12.h>

/// std
#include <memory>
#include <vector>
#include <cstdint>
#include <string>

/// engine
#include "GraphicManager/TextureManager/Texture.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxStructuredBuffer.h"

/// component
#include "ComponentManager/Transform/Transform.h"

/// base class
#include "ComponentManager/Base/BaseComponent.h"


/// using namespace
using namespace Microsoft::WRL;


/// ===================================================
/// マテリアルバッファ用
/// ===================================================
struct NumberRendererMaterial {
	Vec4  color;
	Vec2  textureSize;
	Vec2  tileSize;
	float space;
};

/// ===================================================
/// 頂点バッファ用
/// ===================================================
struct NumberRendererVertexData {
	Vec4 position;
	Vec2 texcoord;
};

/// ===================================================
/// 数字を表示する用のコンポーネント
/// ===================================================
class NumberRenderer final : public BaseComponent {
public:

	NumberRenderer(uint32_t _maxDigit);
	~NumberRenderer();

	void Initialize() override;
	void Update()     override;
	void Draw()       override;
	void Debug()      override;


	/// <summary>
	/// スコアの各桁の値を計算する
	/// </summary>
	void CalcuationScoreDigit();

	/// <summary>
	/// マテリアルをコマンドリストにバインドする
	/// </summary>
	/// <param name="_commandList"></param>
	void MaterialBindToCommandList(UINT _rootParamIndex, ID3D12GraphicsCommandList* _commandList);

	/// <summary>
	/// トランスフォームの配列をコマンドリストにバインドする
	/// </summary>
	/// <param name="_rootParamIndex"></param>
	/// <param name="_commandList"></param>
	void TransformArrayBindToCommandList(UINT _rootParamIndex, ID3D12GraphicsCommandList* _commandList);

	/// <summary>
	/// テクスチャをコマンドリストにバインドする
	/// </summary>
	/// <param name="_rootParamIndex"></param>
	/// <param name="_commandList"></param>
	void TextureBindToCommandList(UINT _rootParamIndex, ID3D12GraphicsCommandList* _commandList);

	/// <summary>
	/// 描画コールまでを行う
	/// </summary>
	/// <param name="_commandList"></param>
	void DrawCall(ID3D12GraphicsCommandList* _commandList);

	void SetTexture(const std::string& _filePath);

	void SetSpace(float _space);
	void SetColor(const Vec4& _color);

	void SetScore(uint32_t _score);
	uint32_t GetScore() const { return score_; }

private:

	uint32_t                                      maxScore_;           /// スコアの最大値
	uint32_t                                      score_;              /// スコア
	uint32_t                                      scoreDigit_;              /// スコア
	const uint32_t                                kMaxDigit_;          /// スコアの桁の最大値

	std::vector<Transform>                        transformArray_;     /// 各数字の座標の配列
	std::unique_ptr<DxStructuredBuffer<Mat4>>     matTransformBuffer_;

	std::vector<uint32_t>                         scoreDigitArray_;    /// 各桁の値を保持する
	std::unique_ptr<DxStructuredBuffer<uint32_t>> numberDigitBuffer_;

	ComPtr<ID3D12Resource>                        materialBuffer_     = nullptr;
	NumberRendererMaterial*                       mappedMaterialData_ = nullptr;

	std::string                                   textureName_;        /// テクスチャの名前

};
