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

#include "NumberRendererCommon.h"


NumberRenderer::NumberRenderer(uint32_t _maxDigit) : kMaxDigit_(_maxDigit) {
	Assert(kMaxDigit_ != 0u, "empty");
	maxScore_ = (kMaxDigit_ + 1) * 10u - 1;
}

NumberRenderer::~NumberRenderer() {
	matTransformBuffer_.reset();
	materialBuffer_.Reset();
}

void NumberRenderer::Initialize() {

	/// ---------------------------------------------------
	/// マテリアルバッファの作成
	/// ---------------------------------------------------

	materialBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(NumberRendererMaterial));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedMaterialData_));

	/// マッピング用データの初期化
	mappedMaterialData_->color       = Vec4::kWhite;
	mappedMaterialData_->textureSize = Vec2(1.0f, 1.0f);
	mappedMaterialData_->tileSize    = Vec2(1.0f, 1.0f);
	mappedMaterialData_->space       = 1.0f;


	/// ---------------------------------------------------
	/// transform bufferの作成
	/// ---------------------------------------------------
	

	transformArray_.resize(kMaxDigit_);

	for(Transform& transform : transformArray_) {
		transform.SetName(std::format("Transform##{:p}", reinterpret_cast<void*>(&transform)));
		transform.SetParent(GetOwner()->GetTransform());
	}

	
	matTransformBuffer_.reset(new DxStructuredBuffer<Mat4>());
	matTransformBuffer_->Create(kMaxDigit_);

	for(size_t i = 0; i < kMaxDigit_; ++i) {
		matTransformBuffer_->SetMappedData(i, transformArray_[i].matTransform);
	}


	numberDigitBuffer_.reset(new DxStructuredBuffer<uint32_t>());
	numberDigitBuffer_->Create(kMaxDigit_);

	scoreDigitArray_.resize(kMaxDigit_);
	for(size_t i = 0; i < kMaxDigit_; ++i) {
		scoreDigitArray_[i] = static_cast<uint32_t>(i);
		numberDigitBuffer_->SetMappedData(i, scoreDigitArray_[i]);
	}

	SetTexture("number.png");

}

void NumberRenderer::Update() {

	/// transformの行列の更新、 行列をバッファのコピー用配列にコピー
	for(size_t i = 0; i < transformArray_.size(); ++i) {
		Transform& transform = transformArray_[i];
		
		transform.position.x = static_cast<float>(i) * mappedMaterialData_->space;
		transform.Update();
		matTransformBuffer_->SetMappedData(i, transformArray_[i].matTransform);
	}

	CalcuationScoreDigit();
	for(size_t i = 0; i < kMaxDigit_; ++i) {
		numberDigitBuffer_->SetMappedData(i, scoreDigitArray_[i]);
	}


}

void NumberRenderer::Draw() {
	NumberRendererCommon::GetInstance()->AddActive(this);
}

void NumberRenderer::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text(std::format("score : {}", score_).c_str());

		ImGui::Spacing();

		ImGui::ColorEdit4("color", &mappedMaterialData_->color.x, ImGuiColorEditFlags_AlphaBar);
		ImGui::DragFloat("space", &mappedMaterialData_->space, 0.01f);

		ImGui::Spacing();

		for(Transform& transform : transformArray_) {
			transform.Debug();
		}

		ImGui::Spacing();

		for(auto& numberDigit : scoreDigitArray_) {
			std::string label = std::format("numberDigit##{:p}", reinterpret_cast<void*>(&numberDigit));
			ImGui::DragInt(label.c_str(), reinterpret_cast<int*>(&numberDigit));
		}

		ImGui::TreePop();
	}
}


void NumberRenderer::CalcuationScoreDigit() {
	//Assert(score_ != 0u, "The score was zero.");

	std::vector<uint32_t> digits;
	uint32_t scoreCopy = score_;

	/// 一の桁から確認
	do {
		digits.push_back(scoreCopy % 10);
		scoreCopy /= 10;
	} while(scoreCopy > 0);

	/// 逆順にする
	std::reverse(digits.begin(), digits.end());

	/// digit arrayに格納
	scoreDigit_ = 0;
	for(size_t i = 0; i < digits.size(); ++i) {
		scoreDigit_++;
		scoreDigitArray_[i] = digits[i];
	}

	if(drawDigit_ != -1) {
		bool isReverse = false;
		for(size_t i = scoreDigit_; i < kMaxDigit_; ++i) {
			scoreDigitArray_[i] = 0;
			isReverse = true;
		}
	
		if(isReverse) {
			std::reverse(scoreDigitArray_.begin(), scoreDigitArray_.end());
		}
	}
}


void NumberRenderer::MaterialBindToCommandList(UINT _rootParamIndex, ID3D12GraphicsCommandList* _commandList) {
	_commandList->SetGraphicsRootConstantBufferView(_rootParamIndex, materialBuffer_->GetGPUVirtualAddress());
}

void NumberRenderer::TransformArrayBindToCommandList(UINT _rootParamIndex, ID3D12GraphicsCommandList* _commandList) {
	matTransformBuffer_->BindToCommandList(_rootParamIndex, _commandList);
}

void NumberRenderer::TextureBindToCommandList(UINT _rootParamIndex, ID3D12GraphicsCommandList* _commandList) {
	const Texture& texture = TextureManager::GetInstance()->GetTexture(textureName_);
	_commandList->SetGraphicsRootDescriptorTable(_rootParamIndex, texture.GetGPUHandle());
}

void NumberRenderer::DrawCall(ID3D12GraphicsCommandList* _commandList) {

	MaterialBindToCommandList(1, _commandList);
	TransformArrayBindToCommandList(2, _commandList);
	TextureBindToCommandList(3, _commandList);
	numberDigitBuffer_->BindToCommandList(4, _commandList);

	if(drawDigit_ == -1) {
		_commandList->DrawIndexedInstanced(6, scoreDigit_ ? scoreDigit_ : 1, 0, 0, 0);
	} else {
		_commandList->DrawIndexedInstanced(6, drawDigit_, 0, 0, 0);
	}
}


void NumberRenderer::SetTexture(const std::string& _filePath) {
	Assert(!_filePath.empty(), "file path emply!!!");

	size_t dotPosition = _filePath.find('.');
	std::string name   = _filePath;

	if(dotPosition != std::string::npos) {
		name.erase(dotPosition);
	}

	textureName_ = name;
	TextureManager::GetInstance()->Load(textureName_, _filePath);
	const Texture& texture = TextureManager::GetInstance()->GetTexture(textureName_);

	mappedMaterialData_->textureSize = texture.GetTextureSize();
	mappedMaterialData_->tileSize    = mappedMaterialData_->textureSize;
	mappedMaterialData_->tileSize.x /= 10.0f; /// 0~9の数が10なので
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

void NumberRenderer::SetDrawDigit(uint32_t _drawDigit) {
	drawDigit_ = _drawDigit;
}
