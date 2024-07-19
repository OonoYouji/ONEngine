#include <ModelManager.h>

#include <DxCommon.h>
#include <DxCommand.h>

#include <PipelineState.h>
#include <Model.h>


/// ===================================================
/// インスタンス確保
/// ===================================================
ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}


/// ===================================================
/// 初期化
/// ===================================================
void ModelManager::Initialize() {

	for(uint8_t i = 0u; i < FillMode::kCount; ++i) {
		pipelines_.push_back(std::make_unique<PipelineState>());
	}

	shader_.ShaderCompile(
		L"Model/Model.VS.hlsl", L"vs_6_0",
		L"Model/Model.PS.hlsl", L"ps_6_0"
	);

	pipelines_[kSolid]->SetFillMode(kSolid);
	pipelines_[kWireFrame]->SetFillMode(kWireFrame);

	for(auto& pipeline : pipelines_) {
		pipeline->SetShader(&shader_);

		pipeline->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- transform
		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- viewProjection

		pipeline->Initialize();
	}


}


/// ===================================================
/// 終了処理
/// ===================================================
void ModelManager::Finalize() {
	models_.clear();
	pipelines_.clear();
}


/// ===================================================
/// 描画前処理
/// ===================================================
void ModelManager::PreDraw() {

}


/// ===================================================
/// 描画後処理
/// ===================================================
void ModelManager::PostDraw() {

	std::list<Model*> solid;
	std::list<Model*> wire;

	/// ---------------------------------------------------
	/// SolidとWireFrameで仕分け
	/// ---------------------------------------------------
	for(const auto& model : models_) {
		if(model->GetFillMode() == FillMode::kSolid) {
			solid.push_back(model.get());
		} else {
			wire.push_back(model.get());
		}
	}



}

void ModelManager::SetPipelineState(FillMode fillMode) {
	pipelines_[fillMode]->SetPipelineState();
}
