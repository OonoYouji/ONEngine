#include "NumberRenderer.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"
#include "GraphicManager/TextureManager/TextureManager.h"

/// lib
#include "Debugger/Assertion.h"


NumberRenderer::NumberRenderer(uint32_t _maxDigit) : kMaxDigit_(_maxDigit) {
	Assert(kMaxDigit_ != 0u, "empty");
	maxScore_ = (kMaxDigit_ + 1) * 10u - 1;
}

NumberRenderer::~NumberRenderer() {
	ONE::DxDescriptorHeap<ONE::HeapType::CBV_SRV_UAV>* pSRVHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();
	pSRVHeap->Free(transformSRVDescriptorIndex_);

	transformArrayBuffer_.Reset();
	materialBuffer_.Reset();
}

void NumberRenderer::Initialize() {

	/// ---------------------------------------------------
	/// マテリアルバッファの作成
	/// ---------------------------------------------------

	materialBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(NumberRendererMaterial));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedMaterialData_));

	/// マッピング用データの初期化
	mappedMaterialData_->color = Vec4::kWhite;
	mappedMaterialData_->space = 16.0f;


	/// ---------------------------------------------------
	/// transform bufferの作成
	/// ---------------------------------------------------
	
	/// buffer initialize
	transformArrayBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4) * kMaxDigit_);

	/// desc setting
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format                     = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement        = 0;
	desc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements         = static_cast<UINT>(kMaxDigit_);
	desc.Buffer.StructureByteStride = sizeof(Mat4);

	/// cpu, gpu handle initialize
	ONE::DxDescriptorHeap<ONE::HeapType::CBV_SRV_UAV>* pSRVHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();
	transformSRVDescriptorIndex_ = pSRVHeap->Allocate();

	transformCPUHandle_ = pSRVHeap->GetCPUDescriptorHandel(transformSRVDescriptorIndex_);
	transformGPUHandle_ = pSRVHeap->GetGPUDescriptorHandel(transformSRVDescriptorIndex_);

	/// resource create
	auto dxDevice = ONEngine::GetDxCommon()->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(transformArrayBuffer_.Get(), &desc, transformCPUHandle_);

	/// mapping data bind
	Mat4* mappedData = nullptr;
	transformArrayBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

}

void NumberRenderer::Update() {}

void NumberRenderer::Draw() {}

void NumberRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(std::format("score : {}", score_).c_str());

		ImGui::Spacing();

		ImGui::ColorEdit4("color", &mappedMaterialData_->color.x, ImGuiColorEditFlags_AlphaBar);
		ImGui::DragFloat("space", &mappedMaterialData_->space, 0.01f);

		ImGui::TreePop();
	}
}


void NumberRenderer::CalcuationScoreDigit() {
	Assert(score_ != 0u, "The score was zero.");

	std::vector<uint32_t> digits;

	/// 一の桁から確認
	do {
		digits.push_back(score_ % 10);
		score_ /= 10;
	} while(score_ > 0);

	/// 逆順にする
	std::reverse(digits.begin(), digits.end());

	/// digit arrayに格納
	scoreDigitArray_ = digits;
}


void NumberRenderer::MaterialBindToCommandList(UINT _rootParamIndex, ID3D12GraphicsCommandList* _commandList) {
	_commandList->SetGraphicsRootConstantBufferView(_rootParamIndex, materialBuffer_->GetGPUVirtualAddress());
}

void NumberRenderer::SetTexture(const std::string& _filePath) {
	Assert(!_filePath.empty(), "file path emply!!!");

	size_t dotPosition = _filePath.find('.');
	std::string name   = _filePath;

	if(dotPosition != std::string::npos) {
		name.erase(dotPosition);
	}

	TextureManager::GetInstance()->Load(name, _filePath);

	textureName_ = name;
}

void NumberRenderer::SetSpace(float _space) {
	mappedMaterialData_->space = _space;
}

void NumberRenderer::SetColor(const Vec4& _color) {
	mappedMaterialData_->color = _color;
}

void NumberRenderer::SetScore(uint32_t _score) {
	score_ = _score;
}
