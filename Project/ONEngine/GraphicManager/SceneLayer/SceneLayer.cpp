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
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"
#include "ComponentManager/NumberRenderer/NumberRendererCommon.h"
	
#include "Objects/Camera/Manager/CameraManager.h"


/// ===================================================
/// namespace
/// ===================================================
namespace {
	CameraManager*           gCameraManager           = CameraManager::GetInstance();
	ModelManager*            gModelManager            = ModelManager::GetInstance();
	SpriteManager*           gSpriteManager           = SpriteManager::GetInstance();
	GameObjectManager*       gGameObjectManager       = GameObjectManager::GetInstance();
	Line2D*                  gLine2D                  = Line2D::GetInstance();
	Line3D*                  gLine3D                  = Line3D::GetInstance();
	AnimationRendererCommon* gAnimationRendererCommon = AnimationRendererCommon::GetInstance();
	NumberRendererCommon*    gNumberRendererCommon    = NumberRendererCommon::GetInstance();
} /// namespace


/// ===================================================
/// static member object initialize
/// ===================================================
int SceneLayer::sInstanceCount_ = 0;

SceneLayer::SceneLayer() {
	id_ = sInstanceCount_++;

	pDxCommand_ = ONEngine::GetDxCommon()->GetDxCommand();
}

void SceneLayer::ResetInstanceCount() {
	sInstanceCount_ = 0;
}


void SceneLayer::Initialize(const std::string& className, BaseCamera* camera) {
	auto pDxCommon = ONEngine::GetDxCommon();
	auto commandList = pDxCommon->GetDxCommand()->GetList();

	renderTexture_.reset(new RenderTexture);
	renderTexture_->Initialize(
		Vec4(0,0,0, 0), commandList, 
		pDxCommon->GetSRVDescriptorHeap(),
		pDxCommon->GetRTVDescriptorHeap(),
		pDxCommon->GetDSVDescriptorHeap()
	);

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

	gLine2D->PreDraw();
	gLine3D->PreDraw();
	gSpriteManager->PreDraw();
	gModelManager->PreDraw();
	gAnimationRendererCommon->PreDraw();
	gNumberRendererCommon->PreDraw();

	gGameObjectManager->Object3dDraw(id_);

	gLine2D->PostDraw();
	gLine3D->PostDraw();
	gSpriteManager->PostDraw();
	gModelManager->PostDraw();
	gAnimationRendererCommon->PostDraw();
	gNumberRendererCommon->PostDraw();

	renderTexture_->EndRenderTarget();


	
	/// command execution.
	pDxCommand_->Close();
	pDxCommand_->Execution();
	pDxCommand_->Reset();



	/// srv heapの再設定
	ONE::DxCommon*             dxCommon     = ONEngine::GetDxCommon();
	ID3D12GraphicsCommandList* pCommandList = dxCommon->GetDxCommand()->GetList();
	dxCommon->GetSRVDescriptorHeap()->BindToCommandList(pCommandList);

}


void SceneLayer::SetMainCamera(BaseCamera* _camera) {
	camera_ = _camera;
}
