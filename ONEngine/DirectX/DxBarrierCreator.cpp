#include <DxBarrierCreator.h>

#include <DxCommand.h>
#include <DxCommon.h>


void ONE::DxBarrierCreator::CreateBarrier(ID3D12Resource* resourec, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	if(before == after) { return; }
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resourec;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;

	ONE::DxCommon::GetInstance()->GetDxCommand()->GetList()->ResourceBarrier(1, &barrier);
}
