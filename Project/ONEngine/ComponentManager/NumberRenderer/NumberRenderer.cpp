#include "NumberRenderer.h"

/// std
#include <format>

/// externals
#include <imgui.h>

/// engine
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/TextureManager/TextureManager.h"

/// lib
#include "Debugger/Assertion.h"


NumberRenderer::NumberRenderer() {}
NumberRenderer::~NumberRenderer() {}

void NumberRenderer::Initialize() {

	/// マテリアルバッファの作成
	materialBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(NumberRendererMaterial));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(mappedMaterialData_));

	/// マッピング用データの初期化
	mappedMaterialData_->color = Vec4::kWhite;
	mappedMaterialData_->space = 16.0f;

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
