#include "ModelRenderer.h"

ModelRenderer::ModelRenderer(ID3D12GraphicsCommandList* _commandList) 
	: BaseRenderer(_commandList) {

}


ModelRenderer::~ModelRenderer() {}



void ModelRenderer::Initialize() {

	/// pipelineの初期化



}


void ModelRenderer::PreDraw() {
	/// pipelineのセット

	pipelineState_->SetPipelineState();

}


void ModelRenderer::PostDraw() {

}