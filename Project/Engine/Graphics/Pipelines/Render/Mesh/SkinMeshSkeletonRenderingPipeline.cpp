#include "SkinMeshSkeletonRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"

using namespace GizmoPrimitive;


SkinMeshSkeletonRenderingPipeline::SkinMeshSkeletonRenderingPipeline() {}

void SkinMeshSkeletonRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/Line/Line3D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// input element setting
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetBlendDesc(BlendMode::None());
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< view projection

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		depthStencilDesc.StencilEnable = FALSE;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}



	{
		/// verticesを最大数分メモリを確保
		maxVertexNum_ = static_cast<size_t>(std::pow(2, 16));
		vertices_.reserve(maxVertexNum_);

		/// vertex bufferの作成
		vertexBuffer_.CreateResource(_dxm->GetDxDevice(), sizeof(VertexData) * maxVertexNum_);
		vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData_));

		vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
		vbv_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * maxVertexNum_);
		vbv_.StrideInBytes = static_cast<UINT>(sizeof(VertexData));
	}

}

void SkinMeshSkeletonRenderingPipeline::Draw(class ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<SkinMeshRenderer>* skinMeshRendererArray = _ecs->GetComponentArray<SkinMeshRenderer>();
	if (!skinMeshRendererArray || skinMeshRendererArray->GetUsedComponents().empty()) {
		return;
	}


	/// 頂点データを集める
	for (auto& smRenderer : skinMeshRendererArray->GetUsedComponents()) {

		float scale = smRenderer->GetOwner()->GetScale().Length();

		for (const Joint& joint : smRenderer->GetSkeleton().joints) {
			if (!joint.parent.has_value()) {
				continue; // ルートジョイントはスキップ
			}

			/// ----------------------------------------
			/// Jointの位置にSphereとRectを描画
			/// ----------------------------------------

			/// 色と座標を取得
			Matrix4x4&& thisWorldMatrix = joint.matSkeletonSpace * smRenderer->GetOwner()->GetTransform()->matWorld;
			Vector3 thisPosition = Matrix4x4::Transform(Vector3::Zero, thisWorldMatrix);
			Vector4 thisColor = Color::kRed;

			/// Sphereの頂点データを取得
			auto sphereVertices = GetSphereVertices(thisPosition, 1.0f * scale, thisColor, 12);
			vertices_.insert(vertices_.end(), sphereVertices.begin(), sphereVertices.end());

			/// Rectの頂点データを取得
			auto rectVertices = GetRectVertices(thisWorldMatrix, Color::kGreen, Vector2::One * (4.0f * scale));
			vertices_.insert(vertices_.end(), rectVertices.begin(), rectVertices.end());


			/// ----------------------------------------
			/// 自身と親Jointを結ぶ線を描画
			/// ----------------------------------------

			const Joint& parentJoint = smRenderer->GetSkeleton().joints[joint.parent.value()];
			Matrix4x4&& parentWorldMatrix = parentJoint.matSkeletonSpace * smRenderer->GetOwner()->GetTransform()->matWorld;
			Vector3 parentPosition = Matrix4x4::Transform(Vector3::Zero, parentWorldMatrix);

			/// 線の頂点データを作成
			VertexData v0, v1;
			v0.position = Vector4(thisPosition, 1.0f);
			v0.color = thisColor;
			v1.position = Vector4(parentPosition, 1.0f);
			v1.color = thisColor;

			vertices_.push_back(v0);
			vertices_.push_back(v1);

		}

	}



	std::memcpy(mappingData_, vertices_.data(), sizeof(VertexData) * vertices_.size());

	/// 描画命令を行う
	auto commandList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(commandList, 0);

	/// draw call
	commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

	vertices_.clear();
}

