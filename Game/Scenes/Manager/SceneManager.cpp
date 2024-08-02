#define NOMINMAX
#include <SceneManager.h>

#include <d3dx12.h>

#include <WinApp.h>
#include <DxCommon.h>
#include <DxCommand.h>
#include <DxDevice.h>
#include <DxDescriptor.h>
#include <DxBarrierCreator.h>

#include <ImGuiManager.h>

#include <BaseScene.h>
#include <Scene_Game.h>



/// ===================================================
/// インスタンス確保
/// ===================================================
SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}


/// ===================================================
/// 初期化処理
/// ===================================================
void SceneManager::Initialize() {
	scene_.reset(new Scene_Game());
	scene_->Initialize();



	ID3D12Device* device = ONE::DxCommon::GetInstance()->GetDevice();
	ONE::DxDescriptor* descriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();

	for(int i = 0; i < 2; ++i) {
		rtvHandles_[i] = descriptor->GetRtvCpuHandle();
		descriptor->AddRtvUsedCount();
	}

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		ONE::WinApp::kWindowSizeX,
		ONE::WinApp::kWindowSizeY,
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearValue.Color[0] = 0.1f;  // R
	clearValue.Color[1] = 0.25f; // G
	clearValue.Color[2] = 0.5f;  // B
	clearValue.Color[3] = 1.0f;  // A

	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	///- render targetの初期化
	for(int i = 0; i < 2; ++i) {
		// リソースを作成
		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_PRESENT,
			&clearValue,
			IID_PPV_ARGS(&renderTargets_[i])
		);

		// RTVを作成
		device->CreateRenderTargetView(renderTargets_[i].Get(), &desc, rtvHandles_[i]);
	}



	/// SRVの作成

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	for(uint32_t index = 0u; index < 2; ++index) {

		srvCPUHandles_[index] = descriptor->GetSrvCpuHandle();
		srvGPUHandles_[index] = descriptor->GetSrvGpuHandle();
		descriptor->AddSrvUsedCount();

		// SRVを作成
		device->CreateShaderResourceView(renderTargets_[index].Get(), &srvDesc, srvCPUHandles_[index]);
	}


}


/// ===================================================
/// 終了処理
/// ===================================================
void SceneManager::Finalize() {
	scene_.reset();
	renderTargets_[0].Reset();
	renderTargets_[1].Reset();
}


/// ===================================================
/// 更新処理
/// ===================================================
void SceneManager::Update() {
	scene_->Update();
}


/// ===================================================
/// 描画処理
/// ===================================================
void SceneManager::Draw() {
	scene_->Draw();
}


/// ===================================================
/// 描画前処理
/// ===================================================
void SceneManager::PreDraw() {
	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = ONE::DxCommon::GetInstance()->GetDxDescriptor()->GetDsvCpuHandle();


	///- present から render targetへ変更
	for(uint8_t index = 0u; index < 2; ++index) {
		ONE::DxBarrierCreator::CreateBarrier(renderTargets_[index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	commandList->OMSetRenderTargets(1, &rtvHandles_[0], FALSE, &dsvHandle);

	const float color[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	for(uint8_t index = 0u; index < 2; ++index) {
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		commandList->ClearRenderTargetView(rtvHandles_[index], color, 0, nullptr);
	}


}

/// ===================================================
/// 描画後処理
/// ===================================================
void SceneManager::PostDraw() {
	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();


	///- render targetから shader resourceへ変更
	for(uint8_t index = 0u; index < 2; ++index) {
		ONE::DxBarrierCreator::CreateBarrier(renderTargets_[index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}



}



void SceneManager::SceneDraw() {
	ImGuiDraw("Game", kCurrentScene);
	ImGuiDraw("Debug", kDebugScene);
}

void SceneManager::ImGuiDraw(const std::string& name, RTVIndex index) {
#ifdef _DEBUG

	//ONE::DxBarrierCreator::CreateBarrier(renderTargets_[index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


	if(!ImGui::Begin(name.c_str())) {
		ImGui::End();
		return;
	}


	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	ImVec2 winSize = {
		max.x - min.x,
		max.y - min.y
	};



	///- 大きさの調整
	ImVec2 texSize = winSize;
	if(texSize.x <= texSize.y) {
		///- x優先
		texSize.y = (texSize.x / 16.0f) * 9.0f;
	} else {
		///- y優先
		float x = (texSize.y / 9.0f) * 16.0f;
		if(x < texSize.x) {
			texSize.x = x;
		} else {
			texSize.y = (texSize.x / 16.0f) * 9.0f;
		}
	}

	ImVec2 texPos = {
		winSize.x * 0.5f,
		winSize.y * 0.5f
	};

	texPos.y -= texSize.y / 2.0f;
	texPos.x -= texSize.x / 2.0f;

	texPos.x = std::max(texPos.x, min.x);
	texPos.y = std::max(texPos.y, min.y);

	ImGui::SetCursorPos(texPos);

	ImTextureID id = ImTextureID(srvGPUHandles_[index].ptr);
	ImGui::Image(id, texSize);
	ImGui::End();
	//ONE::DxBarrierCreator::CreateBarrier(renderTargets_[index].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

#endif // DEBUG
}

void SceneManager::SetRenderTarget(RTVIndex index) {
	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = ONE::DxCommon::GetInstance()->GetDxDescriptor()->GetDsvCpuHandle();
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(1, &rtvHandles_[index], FALSE, &dsvHandle);
}
