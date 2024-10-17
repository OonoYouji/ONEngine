#include "SceneLayer.h"

#include <Core/ONEngine.h>

#include "ImGuiManager/ImGuiManager.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxBarrierCreator.h"

#include "Objects/Camera/Manager/CameraManager.h"
#include "GameObjectManager/GameObjectManager.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/Drawer/Sprite/SpriteManager.h"
#include "GraphicManager/RenderTextureManager/RenderTexture.h"
#include "GraphicManager/TextureManager/TextureManager.h"
#include "GraphicManager/Drawer/LineDrawer/Line2D.h"
#include "GraphicManager/Drawer/LineDrawer/Line3D.h"

#include "Game/CustomComponents/EarthRenderer/EarthRenderer.h"
#include "Objects/Camera/Manager/CameraManager.h"


/// ===================================================
/// namespace
/// ===================================================
namespace {
	CameraManager*     gCameraManager     = CameraManager::GetInstance();
	ModelManager*      gModelManager      = ModelManager::GetInstance();
	SpriteManager*     gSpriteManager     = SpriteManager::GetInstance();
	GameObjectManager* gGameObjectManager = GameObjectManager::GetInstance();
	Line2D*            gLine2D            = Line2D::GetInstance();
	Line3D*            gLine3D            = Line3D::GetInstance();
} /// namespace


/// ===================================================
/// static member object initialize
/// ===================================================
int SceneLayer::sInstanceCount_ = 0;

SceneLayer::SceneLayer() {
	id_ = sInstanceCount_++;
}

void SceneLayer::ResetInstanceCount() {
	sInstanceCount_ = 0;
}


void SceneLayer::Initialize(const std::string& className, BaseCamera* camera) {
	auto commandList = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	auto dxDescriptor = ONEngine::GetDxCommon()->GetDxDescriptor();

	renderTexture_.reset(new RenderTexture);
	renderTexture_->Initialize(Vec4(0, 0, 0, 0), commandList, dxDescriptor);

	camera_    = camera;
	className_ = className;

	TextureManager::GetInstance()->AddTexture(
		className_,
		renderTexture_->GetSrvCpuHandle(),
		renderTexture_->GetSrvGpuHandle()
	);
}


void SceneLayer::Draw() {

	auto debugCamera = gCameraManager->GetCamera("DebugCamera");
	if(!debugCamera->isActive) {
		gCameraManager->SetMainCamera(camera_);
	}

	renderTexture_->BeginRenderTarget();

	EarthRenderer::PreDraw();
	gLine2D->PreDraw();
	gLine3D->PreDraw();
	gSpriteManager->PreDraw();
	gModelManager->PreDraw();

	gGameObjectManager->Object3dDraw(id_);

	EarthRenderer::PostDraw();
	gLine2D->PostDraw();
	gLine3D->PostDraw();
	gSpriteManager->PostDraw();
	gModelManager->PostDraw();

	renderTexture_->EndRenderTarget();

	

}

void SceneLayer::ImGuiDebug() {
	if(!ImGui::TreeNodeEx(className_.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		return;
	}

	for(uint8_t i = 0; i < LAYERNUM_COUNTER; ++i) {
		/*if(blooms_[i] && isApplyBlooms_[i]) {
			std::string treeNodeName = className_ + "_" + std::to_string(i);
			blooms_[i]->ImGuiDebug(treeNodeName);
			if(ImGui::Button(("is delete ???" + std::to_string(i)).c_str())) {
				SetIsApplyBloom(false, LAYER_NUMBER(i));
			}
		} else {
			std::string text = "non create_" + std::to_string(i);
			ImGui::Text(text.c_str());
			if(ImGui::Button(("is create ???" + std::to_string(i)).c_str())) {
				SetIsApplyBloom(true, LAYER_NUMBER(i));
			}
		}*/

		ImGui::Separator();
	}

	ImGui::TreePop();
}


void SceneLayer::SetIsApplyBloom(bool isApplyBloom, LAYER_NUMBER layerNumber) {
	if(blooms_[layerNumber]) {
		isApplyBlooms_[layerNumber] = isApplyBloom;
		return;
	}

	isApplyBlooms_[layerNumber] = isApplyBloom;


	blooms_[layerNumber].reset(new Bloom);
	blooms_[layerNumber]->Initialize();

}

void SceneLayer::SetTexSize(const Vec2& texSize, LAYER_NUMBER layerNumber) {
	blooms_[layerNumber]->SetTexSize(texSize);
}

void SceneLayer::SetIntensity(float intensity, LAYER_NUMBER layerNumber) {
	blooms_[layerNumber]->SetIntensity(intensity);
}

void SceneLayer::SetRadius(int radius, LAYER_NUMBER layerNumber) {
	blooms_[layerNumber]->SetRadius(radius);
}
