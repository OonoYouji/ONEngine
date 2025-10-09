#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/StructuredBuffer.h"

struct BladeInstance {
	Vector3 position;
	Vector3 tangent;
	float scale;
	float random01;
};

/// ////////////////////////////////////////////////////////
/// Terrainに生やすための草の群クラス
/// ////////////////////////////////////////////////////////
class GrassField {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GrassField();
	~GrassField();


	/// 草のバッファを初期化する
	void Initialize(
		uint32_t _maxBladeCount,
		DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap
	);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	StructuredBuffer<BladeInstance> rwBladeInstancesBuffer_;

	uint32_t maxBladeCount_; ///< 最大草の本数

public:
	/// ===================================================
	/// public : accessors
	/// ===================================================

	/// 草のインスタンスバッファの取得
	StructuredBuffer<BladeInstance>& GetRwBladeInstancesBuffer();

	/// 最大草の本数の取得
	uint32_t GetMaxBladeCount() const;

};

