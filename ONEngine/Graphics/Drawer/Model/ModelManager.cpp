#include <ModelManager.h>

#include <DxCommon.h>
#include <DxCommand.h>
#include <DxShaderCompiler.h>
#include <DxResourceCreator.h>

#include <CameraManager.h>

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
		pipeline->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- transform
		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);		///- material

		pipeline->Initialize();
	}


	viewProjectionBuffer_ = ONE::DxResourceCreate::CreateResource(sizeof(ViewProjectionData));
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&viewProjectionData_));
	viewProjectionData_->matVp = Mat4::kIdentity;

}


/// ===================================================
/// 終了処理
/// ===================================================
void ModelManager::Finalize() {
	viewProjectionBuffer_.Reset();
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


	ID3D12GraphicsCommandList* commandList = ONE::DxCommon::GetInstance()->GetCommand()->GetList();
	viewProjectionData_->matVp = CameraManager::GetInstance()->GetMainCamera()->GetMatVp();

	/// ---------------------------------------------------
	/// Solidの描画
	/// ---------------------------------------------------
	
	pipelines_[kSolid]->SetPipelineState();

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, viewProjectionBuffer_->GetGPUVirtualAddress());

	for(auto& model : solid) {
		model->DrawCall(commandList);
	}


	/// ---------------------------------------------------
	/// WireFrameの描画
	/// ---------------------------------------------------

	pipelines_[kWireFrame]->SetPipelineState();
	for(auto& model : wire) {
		model->DrawCall(commandList);
	}


}


/// ===================================================
/// Modelの追加
/// ===================================================
void ModelManager::AddModel(Model* model) {
	std::unique_ptr<Model> add(model);
	models_.push_back(std::move(add));
}


/// ===================================================
/// pipelineのセット
/// ===================================================
void ModelManager::SetPipelineState(FillMode fillMode) {
	pipelines_[fillMode]->SetPipelineState();
}
