#pragma once

/// engine
#include "../Interface/ECSISystem.h"

/// /////////////////////////////////////////////////
/// カスタムメッシュのバッファを再作成するシステム
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class MeshBufferRecreate
 * @brief カスタムメッシュ（CustomMeshRendererなど動的に頂点・インデックス配列が書き換わるもの）のGPU側頂点/インデックスバッファ再生成とデータ転送を同期するシステムクラス
 */
class MeshBufferRecreate final : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ
	 * @param _dxDevice GPUリソース生成用の DirectX12 デバイス
	 */
	MeshBufferRecreate(class DxDevice* _dxDevice);

	/**
	 * @brief デストラクタ
	 */
	~MeshBufferRecreate() = default;

	/**
	 * @brief カスタムメッシュのバッファ更新・再生成処理を毎フレーム実行します。
	 */
	void RuntimeUpdate(class ECSGroup* _ecs) override;


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class DxDevice* pDxDevice_ = nullptr;
};


} /// ONEngine
