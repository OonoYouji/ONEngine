#include "BaseLayer.h"

#include <DxCommon.h>
#include <DxCommand.h>

#include <GameObjectManager.h>
#include <ModelManager.h>
#include <SpriteManager.h>
#include <RenderTextureManager.h>

namespace {

	ModelManager* gModelManager = ModelManager::GetInstance();
	SpriteManager* gSpriteManager = SpriteManager::GetInstance();
	GameObjectManager* gGameObjectManager = GameObjectManager::GetInstance();

} /// namespace



int BaseLayer::sInstanceCount_ = 0;

BaseLayer::BaseLayer() {
	id_ = sInstanceCount_++;
}

void BaseLayer::BaseInitialize() {
	auto commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	auto dxDescriptor = ONE::DxCommon::GetInstance()->GetDxDescriptor();

	for(auto& renderTex : renderTextures_) {
		renderTex.reset(new RenderTexture);
		renderTex->Initialize(Vec4(0, 0, 0, 0), commandList, dxDescriptor);
	}

	finalRenderTex_.reset(new RenderTexture);
	finalRenderTex_->Initialize(Vec4(0, 0, 0, 0), commandList, dxDescriptor);

}

void BaseLayer::Draw() {
	auto commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	renderTextures_[BACK_SPRITE]->BeginRenderTarget();
	gSpriteManager->PreDraw();
	//// 背景スプライトの描画
	gGameObjectManager->BackSpriteDraw(id_);
	gSpriteManager->PostDraw();
	renderTextures_[BACK_SPRITE]->EndRenderTarget();

	renderTextures_[OBJECT3D]->BeginRenderTarget();
	gModelManager->PreDraw();
	//// 3dオブジェクトの描画
	gGameObjectManager->Object3dDraw(id_);
	gModelManager->PostDraw();
	renderTextures_[OBJECT3D]->EndRenderTarget();

	renderTextures_[FRONT_SPRITE]->BeginRenderTarget();
	gSpriteManager->PreDraw();
	//// 前景スプライトの描画
	gGameObjectManager->FrontSpriteDraw(id_);
	gSpriteManager->PostDraw();
	renderTextures_[FRONT_SPRITE]->EndRenderTarget();

	RenderTextureManager::CreateBlendRenderTexture(
		{ renderTextures_[BACK_SPRITE].get(),renderTextures_[OBJECT3D].get(),renderTextures_[FRONT_SPRITE].get()},
		finalRenderTex_.get()
	);
}
