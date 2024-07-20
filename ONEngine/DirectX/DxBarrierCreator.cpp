#include <DxBarrierCreator.h>

#include <DxCommon.h>
#include <DxCommand.h>


void ONE::DxBarrierCreator::CreateBarrier(ID3D12Resource* resourec, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resourec;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;

	ONE::DxCommon::GetInstance()->GetDxCommand()->GetList()->ResourceBarrier(1, &barrier);
}
