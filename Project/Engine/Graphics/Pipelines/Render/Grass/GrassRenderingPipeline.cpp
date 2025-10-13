#include "GrassRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"

Vector3 EvaluateCubicBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	float u = 1.0f - t;
	float b0 = u * u * u;
	float b1 = 3 * u * u * t;
	float b2 = 3 * u * t * t;
	float b3 = t * t * t;
	return (b0 * p0) + (b1 * p1) + (b2 * p2) + (b3 * p3);
}

Vector3 EvaluateCubicBezierTangent(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	float u = 1.0f - t;
	Vector3 d = 3.0f * u * u * (p1 - p0)
		+ 6.0f * u * t * (p2 - p1)
		+ 3.0f * t * t * (p3 - p2);
	// 正規化は呼び出し側で行うかここで行う（ここでは行う）
	// Vector3 に normalize がある想定
	return d.Normalize();
}

void GenerateBladesAlongBezier(
	StructuredBuffer<GrassInstance>& _blades,
	const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3,
	size_t bladeCount, float jitter = 0.0f) {

	std::mt19937 rng(123456); // 固定シード（必要に応じて変更）
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	for (size_t i = 0; i < bladeCount; ++i) {
		// パラメータ t の均等分布＋少しノイズ
		float base = (static_cast<float>(i) + 0.5f) / static_cast<float>(bladeCount);
		float t = base + (jitter > 0.0f ? (dist(rng) - 0.5f) * jitter : 0.0f);
		t = std::clamp(t, 0.0f, 1.0f);

		GrassInstance b{};
		b.position = EvaluateCubicBezier(p0, p1, p2, p3, t);
		b.tangent = EvaluateCubicBezierTangent(p0, p1, p2, p3, t).Normalize();
		b.scale = 0.8f + 0.4f * dist(rng);
		b.random01 = dist(rng);
		_blades.SetMappedData(i, b);
	}
}


GrassRenderingPipeline::GrassRenderingPipeline() = default;
GrassRenderingPipeline::~GrassRenderingPipeline() = default;

void GrassRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);

		/// mesh shader
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.as.hlsl", L"as_6_5", Shader::Type::as, L"ASMain");
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.ms.hlsl", L"ms_6_5", Shader::Type::ms, L"MSMain");
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.ps.hlsl", L"ps_6_0", Shader::Type::ps, L"PSMain");

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// ROOT_PARAM_VIEW_PROJECTION
		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_ALL, 1, 2); // ROOT_PARAM_START_INDEX

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // t0
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // t1

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // ROOT_PARAM_BLADES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // ROOT_PARAM_TIME


		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

}

void GrassRenderingPipeline::Draw(ECSGroup* _ecs, const std::vector<class GameEntity*>& /*_entities*/, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// ================================================
	/// 早期リターンの条件チェック
	/// ================================================
	ComponentArray<GrassField>* grassArray = _ecs->GetComponentArray<GrassField>();
	if (!grassArray) {
		return;
	}

	/// 空チェック
	if (grassArray->GetUsedComponents().empty()) {
		return;
	}


	/// ================================================
	/// 描画に必要なデータの準備
	/// ================================================

	/// 描画コマンドをセット
	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// カメラのBufferを設定
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, ROOT_PARAM_VIEW_PROJECTION);


	for (auto& grass : grassArray->GetUsedComponents()) {
		/// 草が無効化されている場合はスキップ
		if (!grass->enable) {
			continue;
		}

		grass->UpdateTimeBuffer(1.0f / 60.0f);

		/// 草のBufferを設定
		grass->GetRwGrassInstanceBuffer().SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_BLADES);
		grass->GetTimeBuffer().SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_TIME);

		//UINT numThreadsX = 32; // numthreads.x の値
		UINT maxInstancesPerBuffer = static_cast<UINT>(std::pow(2, 16) - 1); // 1つのバッファで処理可能な最大インスタンス数
		UINT instanceCount = static_cast<UINT>(grass->GetMaxGrassCount());
		//UINT bufferCount = (instanceCount + maxInstancesPerBuffer - 1) / maxInstancesPerBuffer;

		for (UINT i = 0; i < 1/*bufferCount*/; ++i) {
			UINT startIndex = i * maxInstancesPerBuffer;
			UINT currentInstanceCount = (std::min)(maxInstancesPerBuffer, instanceCount - startIndex);

			UINT constants[2] = { startIndex, currentInstanceCount };
			cmdList->SetGraphicsRoot32BitConstants(ROOT_PARAM_CONSTANTS, 2, constants, 0);

			// スレッドグループ数を計算
			//UINT threadGroupCountX = (currentInstanceCount + numThreadsX - 1) / numThreadsX; // ceil(currentInstanceCount / numThreadsX)

			// DispatchMeshを呼び出す
			//cmdList->DispatchMesh(1, 1, 1);
		}

	}

}

