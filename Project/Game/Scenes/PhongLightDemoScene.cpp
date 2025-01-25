#include "PhongLightDemoScene.h"

/// engine
#include "SceneManager/SceneManager.h"
#include "Input/Input.h"
#include "GraphicManager/Light/LightGroup.h"

/// user
#include "Objects/Camera/GameCamera.h"
#include "GraphicManager/Light/DirectionalLight.h"
#include "Objects/DemoObject/DemoObject.h"



void PhongLightDemoScene::Initialize() {
	
	{	/// lightの設定
		LightGroup* lightGroup = SceneManager::GetInstance()->GetLightGroup();

		directionalLight_->SetDirection(Vec3(2.0f, -1.0f, 1.0f).Normalize());

		lightGroup->SetDirectionalLightBufferData(0, directionalLight_->GetData());
	}

	{	/// objectの設定
		DemoObject* demoObject = new DemoObject();
		demoObject->Initialize();
		demoObject->SetPosition(Vec3(0.0f, 0.0f, 0.0f));

		mainCamera_->SetPosition({ 0.0f, 2.5f, -10.0f});
		mainCamera_->SetRotateX(0.25f);
	}
}

void PhongLightDemoScene::Update() {

	{	/// lightの設定
		LightGroup* lightGroup = SceneManager::GetInstance()->GetLightGroup();
		lightGroup->SetDirectionalLightBufferData(0, directionalLight_->GetData());
	}
}
