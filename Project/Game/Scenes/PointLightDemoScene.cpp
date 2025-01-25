#include "PointLightDemoScene.h"

/// engine
#include "SceneManager/SceneManager.h"
#include "Input/Input.h"
#include "GraphicManager/Light/LightGroup.h"
#include "GraphicManager/Light/PointLight.h"

/// user
#include "Objects/Camera/GameCamera.h"
#include "Objects/DemoObject/DemoObject.h"


void PointLightDemoScene::Initialize() {

	{	/// lightの設定
	
		/// light groupの instance を取得
		LightGroup* lightGroup = SceneManager::GetInstance()->GetLightGroup();
		/// spot light の instance を生成
		for (auto& spotLight : pointLights_) {
			spotLight = new PointLight();
			spotLight->Initialize();
		}
		for (size_t i = 0; i < 5; i++) {
			lightGroup->SetPointLightBufferData(i, pointLights_[i]->GetData());
		}

	}



	{	/// objectの設定

		for (size_t r = 0; r < 8; r++) {
			for (size_t c = 0; c < 8; c++) {
				DemoObject* demoObject = new DemoObject();
				demoObject->Initialize();

				demoObject->SetPosition(Vec3(
					static_cast<float>(r) * 5.0f,
					0.0f,
					static_cast<float>(c) * 5.0f
				));

				demoObject->UpdateMatrix();
			}
		}

		mainCamera_->SetPosition({ 20.0f, 25.0f, -50.0f });
		mainCamera_->SetRotateX(0.35f);
	}

}


void PointLightDemoScene::Update() {

	{	/// 毎フレーム　light のセットを行う
		LightGroup* lightGroup = SceneManager::GetInstance()->GetLightGroup();
	
		for (size_t i = 0; i < 5; i++) {
			lightGroup->SetPointLightBufferData(i, pointLights_[i]->GetData());
		}
	}
}

