#include <DxCommand.h>

#include <cassert>


ONE::DxCommand::DxCommand() {}
ONE::DxCommand::~DxCommand() {
	list_.Reset();
	allocator_.Reset();
	queue_.Reset();
}


/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxCommand::Initialize(ID3D12Device* device) {

	HRESULT hr = S_FALSE;

	/// ---------------------------------------------------
	/// command queue
	/// ---------------------------------------------------
	D3D12_COMMAND_QUEUE_DESC desc{};
	hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue_));
	assert(SUCCEEDED(hr));



	/// ---------------------------------------------------
	/// command allocator
	/// ---------------------------------------------------
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator_));
	assert(SUCCEEDED(hr));


	/// ---------------------------------------------------
	/// command list
	/// ---------------------------------------------------
	hr = device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		allocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&list_)
	);
	assert(SUCCEEDED(hr));


}



