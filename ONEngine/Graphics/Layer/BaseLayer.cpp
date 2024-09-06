#include "BaseLayer.h"

#include <DxCommon.h>
#include <DxCommand.h>



void BaseLayer::Draw() {
	auto commandList = ONE::DxCommon::GetInstance()->GetDxCommand()->GetList();
	renderTexture_->BeginRenderTarget();
	renderTexture_->EndRenderTarget();
}
