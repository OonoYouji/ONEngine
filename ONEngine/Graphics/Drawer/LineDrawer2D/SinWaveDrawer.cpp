#include "SinWaveDrawer.h"
#include <WinApp.h>
#include <ImGuiManager.h>
#include <Input.h>

SinWaveDrawer::SinWaveDrawer() {
	CreateTag(this);
}

SinWaveDrawer::~SinWaveDrawer() {
	waveHeights_.clear();
}

void SinWaveDrawer::Initialize() {


}

void SinWaveDrawer::Update() {
	waveHeights_.clear();
	int screeSize = ONE::WinApp::kWindowSizeX;


	padLstick = Input::GetInsatnce()->GetLStick();
	padRstick = Input::GetInsatnce()->GetRStick();

	if (padLstick.y > 0 && (!(padLstick.x > 15191) && !(padLstick.x < -15191)))
	{
		float stickSlope = padLstick.y / 16384.0f;
		amplitude += addPadAmp * (stickSlope * stickSlope);
		if (amplitude >= 300)
		{
			amplitude = 300;
		}
	}
	else if (padLstick.y < 0 && (!(padLstick.x > 15191) && !(padLstick.x < -15191)))
	{
		float stickSlope = -(padLstick.y) / 16384.0f;
		amplitude -= addPadAmp * (stickSlope * stickSlope);
		if (amplitude <= 4.0f)
		{
			amplitude = 4.0f;
		}
	}


	if (padRstick.x > 0 && (!(padRstick.y > 17191) && !(padRstick.y < -17191)))
	{
		addlambda -= addPadLam;
		if (addlambda <= -1393.0f)
		{
			addlambda = 0;
		}
	}
	else if (padRstick.x < 0 && (!(padRstick.y > 17191) && !(padRstick.y < -17191)))
	{
		addlambda += addPadLam;
		if (addlambda >= 1393.0f)
		{
			addlambda = 0;
		}
	}



	for (int i = 0; i <= screenOfDivisions; i++) {
		float height = 0;
		float x = (static_cast<float>(screeSize) / static_cast<float>(screenOfDivisions)) * i;

		height = amplitude * sinf(frequency * (x + addlambda)) + offsetY;

		waveHeights_.push_back(height);
	}

	separateLenght_ = static_cast<float>(screeSize) / static_cast<float>(screenOfDivisions);


}

void SinWaveDrawer::Draw() {

}

void SinWaveDrawer::FrontSpriteDraw()
{
	for (int i = 1; i < waveHeights_.size(); i++)
	{
		LineDrawer2D::GetInstance()->Draw({ float(i - 1) * separateLenght_,waveHeights_[i - 1] }, { float(i) * separateLenght_,waveHeights_[i] }, Vector4(0.184f, 0.851f, 0.137f, 1.0f));
	}
}

void SinWaveDrawer::Debug()
{
	if (ImGui::TreeNodeEx("SinWave", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("GamePadLStick");
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 40);
		ImGui::VSliderFloat("v", ImVec2(60, 160), &padLstick.y, -32767.0f, 32767.0f, "%.2f");
		ImGui::PopStyleVar();
		ImGui::SeparatorText("SinWaveSetting");
		ImGui::DragFloat("Amplitude", &amplitude, 0.1f, 4.0f, 300.0f); // 振幅
		ImGui::DragFloat("Frequency", &frequency, 0.0001f, 0.0f, 0.25f); // 周波数
		ImGui::DragFloat("Offset", &offsetY, 0.01f, 0.0f, 720.0f); // 波の振幅セロの時の線の位置
		ImGui::DragInt("Division", &screenOfDivisions, 0.1f, 150, 400); // 分割数
		ImGui::DragFloat("lambda", &addlambda, 1.0f); // 横にずらす値
		ImGui::DragFloat("PadLStick Move", &addPadAmp, 0.01f, 0.0f, 10.0f); // スティック倒した時に波の振幅の増える値
		ImGui::DragFloat("PadRStick Move", &addPadLam, 0.01f, 0.0f, 10.0f); // スティックで横に動く量
		ImGui::TreePop();
	}

}

float SinWaveDrawer::GetAmplitude()
{
	return amplitude;
}

float SinWaveDrawer::GetFrequency()
{
	return frequency;
}

float SinWaveDrawer::GetOffset()
{
	return offsetY;
}

float SinWaveDrawer::GetAddLambda()
{
	return addlambda;
}

float SinWaveDrawer::GetAddLabdaCount()
{
	return addPadLam;
}
