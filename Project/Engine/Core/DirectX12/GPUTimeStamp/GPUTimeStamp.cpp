#include "GPUTimeStamp.h"

#include "../Device/DxDevice.h"
#include "../Command/DxCommand.h"
#include "Engine/Core/Utility/Utility.h"

using namespace ONEngine;

void GPUTimeStamp::Initialize(DxDevice* dxDevice, DxCommand* dxCommand) {

	maxTimerCount_ = static_cast<uint32_t>(GPUTimeStampID::Count);
	pDxCommand_ = dxCommand;

	uint32_t totalQueryCount = maxTimerCount_ * kTimestampPerTimer;

	D3D12_QUERY_HEAP_DESC desc = {};
	desc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
	desc.Count = totalQueryCount;
	desc.NodeMask = 0;

	auto device = dxDevice->GetDevice();

	HRESULT hr = device->CreateQueryHeap(&desc, IID_PPV_ARGS(&queryHeap_));
	Assert(SUCCEEDED(hr), "");

	size_t bufferSize = sizeof(uint64_t) * totalQueryCount;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_READBACK;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = bufferSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	readBackResource_.CreateCommittedResource(
		dxDevice, &heapProps, D3D12_HEAP_FLAG_NONE,
		&resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr
	);

	hr = dxCommand->GetCommandQueue()->GetTimestampFrequency(&gpuTimestampFrequency_);
	Assert(SUCCEEDED(hr), "");
}

void GPUTimeStamp::BeginTimeStamp(GPUTimeStampID id) {
	CheckOutOfRange(id);
	uint32_t index = GetIndex(id);
	pDxCommand_->GetCommandList()->EndQuery(queryHeap_.Get(), D3D12_QUERY_TYPE_TIMESTAMP, index);
}

void GPUTimeStamp::EndTimeStamp(GPUTimeStampID id) {
	CheckOutOfRange(id);
	uint32_t index = GetIndex(id) + 1;
	pDxCommand_->GetCommandList()->EndQuery(queryHeap_.Get(), D3D12_QUERY_TYPE_TIMESTAMP, index);

	uint32_t startIndex = GetIndex(id);
	uint32_t count = kTimestampPerTimer;
	uint64_t dstOffset = startIndex * sizeof(uint64_t);

	pDxCommand_->GetCommandList()->ResolveQueryData(
		queryHeap_.Get(),
		D3D12_QUERY_TYPE_TIMESTAMP,
		startIndex,
		count,
		readBackResource_.Get(),
		dstOffset
	);
}

double GPUTimeStamp::GetTimeStampMSec(GPUTimeStampID id) {
	CheckOutOfRange(id);

	uint64_t* data = nullptr;
	D3D12_RANGE readRange = {};

	readRange.Begin = 0;
	readRange.End = sizeof(uint64_t) * maxTimerCount_ * kTimestampPerTimer;

	HRESULT hr = readBackResource_.Get()->Map(0, &readRange, reinterpret_cast<void**>(&data));
	if(FAILED(hr)) {
		return 0.0;
	}

	uint32_t index = GetIndex(id);
	uint64_t startTime = data[index];
	uint64_t endTime = data[index + 1];

	readBackResource_.Get()->Unmap(0, nullptr);

	/// 例外処理()
	if(startTime == 0 || endTime == 0) {
		return -1.0;
	}

	if(endTime < startTime) {
		return -1.0;
	}

	uint64_t delta = endTime - startTime;
	return (static_cast<double>(delta) / static_cast<double>(gpuTimestampFrequency_)) * 1000.0;
}

uint32_t GPUTimeStamp::GetIndex(GPUTimeStampID id) {
	return static_cast<uint32_t>(id) * kTimestampPerTimer;
}

void GPUTimeStamp::CheckOutOfRange(GPUTimeStampID id) {
	Assert(static_cast<uint32_t>(id) < maxTimerCount_);
}

