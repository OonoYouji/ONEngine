#include "SinWaveDrawer.h"
#include <WinApp.h>
#include <ImGuiManager.h>
#include <Input.h>

SinWaveDrawer::SinWaveDrawer() {}

SinWaveDrawer::~SinWaveDrawer() {
	waveHeights_.clear();
}

void SinWaveDrawer::Initialize() {

	sprite_.reset(new Sprite());
	sprite_->Initialize("uvChecker", "uvChecker.png");

}

void SinWaveDrawer::Update() {
	waveHeights_.clear();
	int screeSize = ONE::WinApp::kWindowSizeX;

	beforPos = pos;
	beforlambda = addlambda;
	isJump = true;

	Vector2 padLstick = Input::GetInsatnce()->GetLStick();
	Vector2 padRstick = Input::GetInsatnce()->GetRStick();

#ifdef _DEBUG
	ImGui::Begin("SinWave Setting");
	ImGui::DragFloat("Amplitude", &amplitude, 0.1f, -300.0f, 300.0f); // 振幅
	ImGui::DragFloat("Frequency", &frequency, 0.0001f, 0.0f, 0.25f); // 周波数
	ImGui::DragFloat("Offset", &offsetY, 0.01f, 0.0f, 720.0f); // 波の振幅セロの時の線の位置
	ImGui::DragInt("Division", &screenOfDivisions, 0.1f, 150, 400); // 分割数
	ImGui::DragFloat("lambda", &addlambda, 1.0f); // 横にずらす値
	if (ImGui::Button("acce")) // 横の加速値をリセット
	{
		xAccel = 0.0f;
	}
	ImGui::DragFloat("Pos", &pos.x, 1.0f); // イラストの位置
	ImGui::DragFloat("PadLStick Move", &addPadAmp, 0.01f, 0.0f, 10.0f); // スティック倒した時に波の振幅の増える値
	ImGui::DragFloat("PadRStick Move", &addPadLam, 0.01f, 0.0f, 10.0f); // スティックで横に動く量
	ImGui::End();
#endif // _DEBUG

	if (padLstick.y > 0)
	{
		amplitude+=addPadAmp;
		if (amplitude >= 300)
		{
			amplitude = 300;
		}
	}
	else if (padLstick.y < 0)
	{
		amplitude-= addPadAmp;
		if (amplitude <= 0)
		{
			amplitude = 0;
		}
	}


	if (padRstick.x > 0)
	{
		addlambda -= addPadLam;
	}
	else if (padRstick.x < 0)
	{
		addlambda += addPadLam;
	}


	pos.x -= (0.02f + xAccel);


	if (beforlambda != addlambda)
	{
		isJump = false;
	}




	for (int i = 0; i <= screenOfDivisions; i++) {
		float height = 0;
		float x = (static_cast<float>(screeSize) / static_cast<float>(screenOfDivisions)) * i;

		height = amplitude * sinf(frequency * (x + addlambda)) + offsetY;

		waveHeights_.push_back(height);
	}

	separateLenght_ = static_cast<float>(screeSize) / static_cast<float>(screenOfDivisions);




	if (!isfly)
	{
		pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
	}
	else
	{
		flyspeed.y += 0.2f;
		pos.y += flyspeed.y;
		if (pos.y >= amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY)
		{
			pos.y = amplitude * sinf(frequency * (pos.x + addlambda)) + offsetY;
			isfly = false;
		}
	}

	sprite_->SetPos(pos);
	if (amplitude >= (pos.y - offsetY) && xAccel > 2.0f && isJump)
	{
		if (!isfly)
		{
			isfly = true;
			flyspeed = pos - beforPos;
		}
	}

	if (!isfly)
	{
		if (beforPos.y < pos.y)
		{
			if (beforlambda != addlambda)
			{
				if (beforlambda > addlambda)
				{
					xAccel += 0.02f;
				}
			}
			else
			{
				xAccel += 0.02f;
			}
		}
	}

	if (pos.x < 0)
	{
		pos.x = 1280;
	}

}

void SinWaveDrawer::Draw() {
	for (int i = 1; i < waveHeights_.size(); i++)
	{
		LineDrawer2D::GetInstance()->Draw({ float(i - 1) * separateLenght_,waveHeights_[i - 1] }, { float(i) * separateLenght_,waveHeights_[i] }, Vector4(0.184f, 0.851f, 0.137f, 1.0f));
	}
	sprite_->Draw();
}
