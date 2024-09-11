#include "SceneLayer.h"

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxBarrierCreator.h>

#include <ImGuiManager.h>
#include <GameObjectManager.h>
#include <ModelManager.h>
#include <SpriteManager.h>
#include <RenderTextureManager.h>
#include <TextureManager.h>
#include <CameraManager.h>
#include <LineDrawer2D/LineDrawer2D.h>

/// ===================================================
/// namespace
/// ===================================================
namespace {
	ModelManager* gModelManager = ModelManager::GetInstance();
	SpriteManager* gSpriteManager = SpriteManager::GetInstance();
	GameObjectManager* gGameObjectManager = GameObjectManager::GetInstance();
	LineDrawer2D* gLineDrawer2d = LineDrawer2D::GetInstance();
} /// namespace


/// ===================================================
/// static member object initialize
/// ===================================================
int SceneLayer::sInstanceCount_ = 0;

SceneLayer::SceneLayer() {
	id_ = sInstanceCount_++;
}

void SceneLayer::Initialize(const std::string& className, BaseCamera* camera) {
	auto commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	auto dxDescriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();

	for(auto& renderTex : renderTextures_) {
		renderTex.reset(new RenderTexture);
		renderTex->Initialize(Vec4(0, 0, 0, 0), commandList, dxDescriptor);
	}

	finalRenderTex_.reset(new RenderTexture);
	finalRenderTex_->Initialize(Vec4(0, 0, 0, 0), commandList, dxDescriptor);

	camera_ = camera;
	className_ = className;

	TextureManager::GetInstance()->AddTexture(
		className_,
		finalRenderTex_->GetSrvCpuHandle(),
		finalRenderTex_->GetSrvGpuHandle()
	);
}


void SceneLayer::Draw() {
	auto commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();

	CameraManager::GetInstance()->SetMainCamera(camera_);

	renderTextures_[BACK_SPRITE]->BeginRenderTarget();
	gLineDrawer2d->PreDraw();
	gSpriteManager->PreDraw();
	//// 背景スプライトの描画
	gGameObjectManager->BackSpriteDraw(id_);
	gSpriteManager->PostDraw();
	gLineDrawer2d->PostDraw();
	renderTextures_[BACK_SPRITE]->EndRenderTarget();

	renderTextures_[OBJECT3D]->BeginRenderTarget();
	gLineDrawer2d->PreDraw();
	gModelManager->PreDraw();
	//// 3dオブジェクトの描画
	gGameObjectManager->Object3dDraw(id_);
	gModelManager->PostDraw();
	gLineDrawer2d->PostDraw();
	renderTextures_[OBJECT3D]->EndRenderTarget();

	renderTextures_[FRONT_SPRITE]->BeginRenderTarget();
	gLineDrawer2d->PreDraw();
	gSpriteManager->PreDraw();
	//// 前景スプライトの描画
	gGameObjectManager->FrontSpriteDraw(id_);
	gSpriteManager->PostDraw();
	gLineDrawer2d->PostDraw();
	gLineDrawer2d->PostDraw();
	renderTextures_[FRONT_SPRITE]->EndRenderTarget();


	/// use post effect 
	for(uint8_t i = 0; i < LAYERNUM_COUNTER; ++i) {
		if(isApplyBlooms_[i]) {

			auto bloomRenderTex = blooms_[i]->GetBloomRenderTexture();
			auto copyDestRenderTex = renderTextures_[i].get();

			ONE::DxBarrierCreator::CreateBarrier(copyDestRenderTex->GetRenderTexResource(), copyDestRenderTex->currentResourceState, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			copyDestRenderTex->currentResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

			blooms_[i]->ApplyBloom(copyDestRenderTex);

			ONE::DxBarrierCreator::CreateBarrier(bloomRenderTex->GetRenderTexResource(), bloomRenderTex->currentResourceState, D3D12_RESOURCE_STATE_COPY_SOURCE);
			bloomRenderTex->currentResourceState = D3D12_RESOURCE_STATE_COPY_SOURCE;
			ONE::DxBarrierCreator::CreateBarrier(copyDestRenderTex->GetRenderTexResource(), copyDestRenderTex->currentResourceState, D3D12_RESOURCE_STATE_COPY_DEST);
			copyDestRenderTex->currentResourceState = D3D12_RESOURCE_STATE_COPY_DEST;

			/// bloom -> render textures[i] にコピー
			commandList->CopyResource(copyDestRenderTex->GetRenderTexResource(), bloomRenderTex->GetRenderTexResource());
		}
	}

	RenderTextureManager::CreateBlendRenderTexture(
		{ renderTextures_[BACK_SPRITE].get(),renderTextures_[OBJECT3D].get(),renderTextures_[FRONT_SPRITE].get() },
		finalRenderTex_.get()
	);
}

void SceneLayer::ImGuiDebug() {
#ifdef _DEBUG
	if(!ImGui::Begin(className_.c_str())) {
		ImGui::End();
		return;
	}


	for(uint8_t i = 0; i < LAYERNUM_COUNTER; ++i) {
		if(blooms_[i] && isApplyBlooms_[i]) {
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
		}

		ImGui::Separator();
	}

	ImGui::End();
#endif // _DEBUG
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
