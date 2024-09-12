#define NOMINMAX
#include "Wave.h"

#include <LineDrawer2D/LineDrawer2D.h>
#include <ImGuiManager.h>
#include <WorldTime.h>
#include <Easing.h>


void Wave::Initialize() {
	amplitude_ = 100.0f;
	frequency_ = 0.027f;
	offsetY_ = 360.0f;
}

void Wave::Update() {

	waveHeights_.clear();
	for(int i = 0; i <= 200; i++) {
		float height = 0;
		float x = (1280.0f / 200.0f) * i;

		height = amplitude_ * std::sin(frequency_ * (x + addlambda_));
		height += offsetY_;

		waveHeights_.push_back(height);
	}

}

void Wave::FrontSpriteDraw() {
	float separateLenght_ = 1280.0f / 200.0f;
	for(int i = 1; i < waveHeights_.size(); i++) {
		LineDrawer2D::GetInstance()->Draw(
			{ float(i - 1) * separateLenght_,waveHeights_[i - 1] },
			{ float(i) * separateLenght_,waveHeights_[i] },
			Vector4(0.184f, 0.851f, 0.137f, 1.0f)
		);
	}
}

void Wave::Debug() {
	if(ImGui::TreeNodeEx("wave", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("amplitude", &amplitude_, 0.1f);
		ImGui::DragFloat("frequency", &frequency_, 0.0001f);
		ImGui::DragFloat("addlambda", &addlambda_, 0.1f);
		ImGui::DragFloat("offsetY", &offsetY_, 0.1f);

		ImGui::TreePop();
	}
}
