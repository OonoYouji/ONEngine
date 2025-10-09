#include "Grass.h"


GrassField::GrassField() = default;
GrassField::~GrassField() = default;

void GrassField::Initialize(uint32_t _maxBladeCount, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {
	maxBladeCount_ = _maxBladeCount;
	/// 草のインスタンスバッファの作成
	rwBladeInstancesBuffer_.CreateUAV(
		maxBladeCount_, _dxDevice, _dxCommand, _dxSRVHeap
	);
}

StructuredBuffer<BladeInstance>& GrassField::GetRwBladeInstancesBuffer() {
	return rwBladeInstancesBuffer_;
}

uint32_t GrassField::GetMaxBladeCount() const {
	return maxBladeCount_;
}
