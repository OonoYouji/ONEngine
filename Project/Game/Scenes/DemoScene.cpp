#include "DemoScene.h"

/// engine
#include "SceneManager/SceneManager.h"
#include "GraphicManager/Light/LightGroup.h"
#include "GraphicManager/Light/SpotLight.h"

void DemoScene::Initialize() {

	{	/// lightの設定

		/// light groupの instance を取得
		LightGroup* lightGroup = SceneManager::GetInstance()->GetLightGroup();

		/// spot light の instance を生成
		for (auto& spotLight : spotLights_) {
			spotLight = new SpotLight();
			spotLight->Initialize();
		}

		for (size_t i = 0; i < 5; i++) {
			lightGroup->SetSpotLightBufferData(i, spotLights_[i]->GetData());
		}
	}


	{	/// objectの設定

	}
}

void DemoScene::Update() {

}
