#include "DxResource.h"

using namespace ONEngine;

/// externals
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>

/// engine
#include "../Device/DxDevice.h"
#include "../Command/DxCommand.h"
#include "Engine/Core/Utility/Tools/Assert.h"


DxResource::DxResource() = default;
DxResource::~DxResource() = default;

/**
 * @brief CPUから書き込み可能なアップロードバッファ（定数バッファなど）を生成します。
 */
void DxResource::CreateResource(DxDevice* _dxDevice, size_t _sizeInByte) {
	HRESULT result = S_FALSE;

	/// 256バイトの倍数に切り上げる (ConstantBufferのアライメント制限)
	size_t alignedSize = (_sizeInByte + 255) & ~255;

	/// ヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; /// バッファリソース
	desc.Width = alignedSize;                     /// リソースのサイズ
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	/// リソースの作成
	result = _dxDevice->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);

	Assert(SUCCEEDED(result), "Resource creation failed.");
}

/**
 * @brief UAV（Unordered Access View）として使用可能なデフォルトヒープ上のバッファを生成します。
 */
void DxResource::CreateUAVResource(DxDevice* _dxDevice, class DxCommand* _dxCommand, size_t _sizeInByte) {
	/// ----- UAVリソースとして作成する ----- ///

	HRESULT result = S_FALSE;

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(
		_sizeInByte,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);

	currentState_ = D3D12_RESOURCE_STATE_COMMON;
	_dxDevice->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		currentState_,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);


	CreateBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, _dxCommand);

	Assert(SUCCEEDED(result), "UAV Resource creation failed.");
}

/**
 * @brief GPUのみがアクセス可能なデフォルトヒープ上にバッファを生成します。
 */
void DxResource::CreateDefaultHeap(DxDevice* _dxDevice, DxCommand* _dxCommand, size_t _sizeInByte, D3D12_RESOURCE_STATES initState) {
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT; // ここがポイント

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = _sizeInByte;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT result = _dxDevice->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);
	Assert(SUCCEEDED(result), "Default Heap Resource creation failed.");

	CreateBarrier(
		D3D12_RESOURCE_STATE_COMMON,
		initState, _dxCommand
	);
}

/**
 * @brief CPUから書き込み可能でGPUからも参照可能なアップロードヒープバッファを生成します。
 */
void ONEngine::DxResource::CreateUploadHeap(DxDevice* dxDevice, DxCommand* dxCommand, size_t sizeInByte, D3D12_RESOURCE_STATES initState) {
	D3D12_HEAP_PROPERTIES heapProps{};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = sizeInByte;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT result = dxDevice->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);
	Assert(SUCCEEDED(result), "Default Heap Resource creation failed.");

	CreateBarrier(
		D3D12_RESOURCE_STATE_COMMON,
		initState, dxCommand
	);
}

/**
 * @brief ID3D12Device::CreateCommittedResourceを直接呼び出し、任意のプロパティでバッファを確保します。
 */
void DxResource::CreateCommittedResource(DxDevice* _dxDevice, const D3D12_HEAP_PROPERTIES* _pHeapProperties, D3D12_HEAP_FLAGS _HeapFlags, const D3D12_RESOURCE_DESC* _pDesc, D3D12_RESOURCE_STATES _InitialResourceState, const D3D12_CLEAR_VALUE* _pOptimizedClearValue) {
	currentState_ = _InitialResourceState;

	HRESULT hr = _dxDevice->GetDevice()->CreateCommittedResource(
		_pHeapProperties,
		_HeapFlags,
		_pDesc,
		_InitialResourceState,
		_pOptimizedClearValue,
		IID_PPV_ARGS(&resource_)
	);

	if(!SUCCEEDED(hr)) {
		Console::LogError("[DxResource::CreateCommittedResource] Committed Resource creation failed.");
		Assert(false, "Committed Resource creation failed.");
	}
}

/**
 * @brief レンダーターゲットとして描画可能なテクスチャリソースを生成します。
 */
void DxResource::CreateRenderTextureResource(DxDevice* _dxDevice, const Vector2& _size, DXGI_FORMAT _format, const Vector4& _clearColor) {
	/// ----- RTVとして利用できるようリソースを作成する ----- ///


	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		_format,
		static_cast<UINT64>(_size.x),
		static_cast<UINT64>(_size.y),
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = _format;
	clearValue.Color[0] = _clearColor.x;
	clearValue.Color[1] = _clearColor.y;
	clearValue.Color[2] = _clearColor.z;
	clearValue.Color[3] = _clearColor.w;

	_dxDevice->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource_)
	);
}

/**
 * @brief コンピュートシェーダ（UAV）から書き込み可能なテクスチャリソースを生成します。
 */
void DxResource::CreateUAVTextureResource(DxDevice* _dxDevice, const Vector2& _size, DXGI_FORMAT _format) {
	/// ----- UAV用のテクスチャとして作成する ----- ///

	currentState_ = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		_format,
		static_cast<UINT64>(_size.x),
		static_cast<UINT64>(_size.y),
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
	);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	HRESULT result = _dxDevice->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		currentState_,
		nullptr,
		IID_PPV_ARGS(&resource_)
	);

	Assert(SUCCEEDED(result), "UAV Texture Resource creation failed.");
}

/**
 * @brief 指定された明示的な状態遷移バリアを作成してコマンドリストに記録し、内部状態を追跡します。
 */
void DxResource::CreateBarrier(D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, DxCommand* _dxCommand) {
	::CreateBarrier(resource_.Get(), _before, _after, _dxCommand);
	currentState_ = _after;

	/// ログ出力 (リソース名、Before、After)
	//{
	//	Console::Log("[DxResource::CreateBarrier]");
	//	Console::Log(L" - Name: " + GetD3D12Name(resource_.Get()));
	//	Console::Log(" - Before State: "
	//		+ std::to_string(static_cast<int>(_before)) + " : "
	//		+ std::string(magic_enum::enum_name<D3D12_RESOURCE_STATES>(_before))
	//	);

	//	Console::Log(" - After State: "
	//		+ std::to_string(static_cast<int>(_after)) + " : "
	//		+ std::string(magic_enum::enum_name<D3D12_RESOURCE_STATES>(_after))
	//	);
	//}
}

/**
 * @brief 内部で記録している現在の状態から、指定された変更後の状態への遷移バリアを作成し記録します。
 */
void DxResource::CreateBarrier(D3D12_RESOURCE_STATES _after, DxCommand* _dxCommand) {
	::CreateBarrier(resource_.Get(), currentState_, _after, _dxCommand);

	/// ログ出力 (リソース名、Before、After)
	//{
	//	Console::Log("[DxResource::CreateBarrier]");
	//	Console::Log(L" - Name: " + GetD3D12Name(resource_.Get()));
	//	Console::Log(" - Before State: "
	//		+ std::to_string(static_cast<int>(currentState_)) + " : "
	//		+ std::string(magic_enum::enum_name<D3D12_RESOURCE_STATES>(currentState_))
	//	);

	//	Console::Log(" - After State: "
	//		+ std::to_string(static_cast<int>(_after)) + " : "
	//		+ std::string(magic_enum::enum_name<D3D12_RESOURCE_STATES>(_after))
	//	);
	//}

	currentState_ = _after;
}

/**
 * @brief 内部のID3D12Resource生ポインタを取得します。
 */
ID3D12Resource* DxResource::Get() const {
	return resource_.Get();
}

/**
 * @brief 内部のComPtr<ID3D12Resource>参照を取得します。
 */
ComPtr<ID3D12Resource>& DxResource::GetComPtr() {
	return resource_;
}

/**
 * @brief 現在追跡しているリソース状態を取得します。
 */
D3D12_RESOURCE_STATES DxResource::GetCurrentState() const {
	return currentState_;
}

/**
 * @brief 追跡している現在のリソース状態フラグを強制的に上書き設定します。
 */
void DxResource::SetCurrentState(D3D12_RESOURCE_STATES _state) {
	currentState_ = _state;
}


/**
 * @brief D3D12デバッグオブジェクトの登録名（SetNameされた文字列）を取得します（デバッグ用）。
 */
std::wstring ONEngine::GetD3D12Name(ID3D12Object* _object) {
	UINT size = 0;

	/// まずサイズを調べる
	HRESULT hr = _object->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, nullptr);
	if(FAILED(hr) || size == 0) {
		return L""; // 名前なし
	}

	std::wstring name(size / sizeof(wchar_t), L'\0');

	hr = _object->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, name.data());
	if(FAILED(hr)) {
		return L"";
	}

	/// 末尾の null を削る
	if(!name.empty() && name.back() == L'\0') {
		name.pop_back();
	}

	return name;
}

/**
 * @brief 生のリソースポインタに対して状態遷移（Transition）バリアを記録します。
 */
void ONEngine::CreateBarrier(ID3D12Resource* _resource, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, DxCommand* _dxCommand) {
	/// ----- リソースバリアーの作成 ----- ///

	if(_before == _after) {
		return;
	}

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = _resource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = _before;
	barrier.Transition.StateAfter = _after;

	_dxCommand->GetCommandList()->ResourceBarrier(1, &barrier);
}

/**
 * @brief 複数のDxResourceラッパーオブジェクトに対し、同一の遷移前状態から遷移後状態への一括バリアを適用します。
 */
void ONEngine::CreateBarriers(std::vector<DxResource*>& _resources, D3D12_RESOURCE_STATES _before, D3D12_RESOURCE_STATES _after, DxCommand* _dxCommand) {
	/// ----- 複数リソースのバリアー作成 ----- ///

	std::vector<D3D12_RESOURCE_BARRIER> barriers;
	barriers.reserve(_resources.size());

	for(auto& res : _resources) {
		if(res->GetCurrentState() != _after) {
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = res->Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = _before;
			barrier.Transition.StateAfter = _after;
			barriers.push_back(barrier);
		}

	}

	if(barriers.empty()) {
		return;
	}

	_dxCommand->GetCommandList()->ResourceBarrier(
		static_cast<UINT>(barriers.size()), barriers.data()
	);

	for(auto& res : _resources) {
		res->SetCurrentState(_after);
	}
}

/**
 * @brief 複数のDxResourceラッパーオブジェクトに対し、それぞれが現在保持している状態から指定の状態への一括遷移バリアを適用します。
 */
void ONEngine::CreateBarriers(std::vector<DxResource*>& _resources, D3D12_RESOURCE_STATES _after, DxCommand* _dxCommand) {

	/// ----- 複数リソースのバリアー作成 ----- ///

	std::vector<D3D12_RESOURCE_BARRIER> barriers;
	barriers.reserve(_resources.size());

	for(auto& res : _resources) {
		if(res->GetCurrentState() != _after) {
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = res->Get();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = res->GetCurrentState();
			barrier.Transition.StateAfter = _after;
			barriers.push_back(barrier);
		}

	}

	if(barriers.empty()) {
		return;
	}

	_dxCommand->GetCommandList()->ResourceBarrier(
		static_cast<UINT>(barriers.size()), barriers.data()
	);

	for(auto& res : _resources) {
		res->SetCurrentState(_after);
	}

}
