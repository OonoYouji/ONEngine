#include "SinWaveDrawer.h"
#include <WinApp.h>
#include <ImGuiManager.h>

SinWaveDrawer::SinWaveDrawer() {}

SinWaveDrawer::~SinWaveDrawer() {
	waveHeights_.clear();
}

void SinWaveDrawer::Initialize() {}

void SinWaveDrawer::Update() {
	waveHeights_.clear();
	int screeSize = ONE::WinApp::kWindowSizeX;

#ifdef _DEBUG
	ImGui::Begin("SinWave Setting");
	ImGui::DragFloat("Amplitude", &amplitude, 0.1f, -300.0f, 300.0f);
	ImGui::DragFloat("Frequency", &frequency, 0.0001f, 0.0f, 0.25f);
	ImGui::DragFloat("Offset", &offsetY, 0.01f, 0.0f, 720.0f);
	ImGui::DragInt("Division", &screenOfDivisions, 0.1f, 150, 400);
	ImGui::End();
#endif // _DEBUG


	for (int i = 0; i <= screenOfDivisions; i++) {
		float height = 0;
		float x = (static_cast<float>(screeSize) / static_cast<float>(screenOfDivisions)) * i;

		height = amplitude * sinf(frequency * x) + offsetY;

		waveHeights_.push_back(height);
	}

	separateLenght_ = static_cast<float>(screeSize) / static_cast<float>(screenOfDivisions);

}

void SinWaveDrawer::Draw() {
	for (int i = 1; i < waveHeights_.size(); i++)
	{
		LineDrawer2D::GetInstance()->Draw({ float(i - 1) * separateLenght_,waveHeights_[i - 1] }, { float(i) * separateLenght_,waveHeights_[i] }, Vector4(0.184f, 0.851f, 0.137f, 1.0f));
	}
}
