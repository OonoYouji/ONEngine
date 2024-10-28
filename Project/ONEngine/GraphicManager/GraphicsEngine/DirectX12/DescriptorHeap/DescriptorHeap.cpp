#include "BaseDescriptorHeap.h"

#include "../DxCommon.h"


using namespace ONE;


void DescriptorHeap<HeapType::UAV_CBV_SRV>::Initialize(DxCommon* _dxCommon) {
	pDxCommon_ = _dxCommon;
	
}