#include "SpriteRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"




SpriteRenderingPipeline::SpriteRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: resourceCollection_(_resourceCollection) {}
SpriteRenderingPipeline::~SpriteRenderingPipeline() {}


void SpriteRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// pipeline 

		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Packages/Shader/Render/Sprite/Sprite.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/Sprite/Sprite.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);                  ///< ROOT_PARAM_VIEW_PROJECTION : 0

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< ROOT_PARAM_MATERIAL
		pipeline_->AddDescriptorRange(1, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< ROOT_PARAM_TEXTURES
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< ROOT_PARAM_TRANSFORM
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);       ///< ROOT_PARAM_MATERIAL : 1
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       ///< ROOT_PARAM_TEXTURES   : 2
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 2);      ///< ROOT_PARAM_TRANSFORM : 3

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);         ///< texture sampler

		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipeline_->SetBlendDesc(blendDesc);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}


	{	/// buffer

			/// vertex data
		vertices_ = {
			{ Vector4(-0.5f, 0.5f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
			{ Vector4(0.5f, 0.5f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
			{ Vector4(-0.5f, -0.5f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
			{ Vector4(0.5f, -0.5f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
		};

		indices_ = {
			0, 1, 2, ///< 1面
			2, 1, 3, ///< 2面
		};

		const size_t kVertexDataSize = sizeof(VertexData);

		/// vertex buffer
		vertexBuffer_.CreateResource(_dxManager->GetDxDevice(), kVertexDataSize * vertices_.size());

		vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
		vbv_.SizeInBytes = static_cast<UINT>(kVertexDataSize * vertices_.size());
		vbv_.StrideInBytes = static_cast<UINT>(kVertexDataSize);

		/// index buffer
		indexBuffer_.CreateResource(_dxManager->GetDxDevice(), sizeof(uint32_t) * indices_.size());

		ibv_.BufferLocation = indexBuffer_.Get()->GetGPUVirtualAddress();
		ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
		ibv_.Format = DXGI_FORMAT_R32_UINT;


		/// mapping
		VertexData* vertexData = nullptr;
		vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
		memcpy(vertexData, vertices_.data(), kVertexDataSize * vertices_.size());

		uint32_t* indexData = nullptr;
		indexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
		memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());

	}


	{	/// structured buffer

		transformsBuffer_ = std::make_unique<StructuredBuffer<Matrix4x4>>();
		transformsBuffer_->Create(static_cast<uint32_t>(kMaxRenderingSpriteCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

		materialsBuffer = std::make_unique<StructuredBuffer<GPUMaterial>>();
		materialsBuffer->Create(static_cast<uint32_t>(kMaxRenderingSpriteCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

	}


}

void SpriteRenderingPipeline::Draw(class ECSGroup* _ecsGroup, const std::vector<GameEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<SpriteRenderer>* spriteRendererArray = _ecsGroup->GetComponentArray<SpriteRenderer>();
	if (!spriteRendererArray || spriteRendererArray->GetUsedComponents().empty()) {
		return;
	}


	ID3D12GraphicsCommandList* cmdList = _dxCommand->GetCommandList();

	/// settings
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// vbv, ivb setting
	cmdList->IASetVertexBuffers(0, 1, &vbv_);
	cmdList->IASetIndexBuffer(&ibv_);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	/// ROOT_PARAM_VIEW_PROJECTION : 0
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, ROOT_PARAM_VIEW_PROJECTION);

	/// 先頭の texture gpu handle をセットする
	auto& textures = resourceCollection_->GetTextures();
	const Texture* firstTexture = &textures.front();
	cmdList->SetGraphicsRootDescriptorTable(ROOT_PARAM_TEXTURES, firstTexture->GetSRVGPUHandle());


	/// bufferにデータをセット
	size_t transformIndex = 0;
	for (auto& renderer : spriteRendererArray->GetUsedComponents()) {
		if (!renderer->enable) {
			continue;
		}

		materialsBuffer->SetMappedData(
			transformIndex, renderer->GetMaterial()
		);

		transformsBuffer_->SetMappedData(
			transformIndex,
			renderer->GetOwner()->GetTransform()->GetMatWorld()
		);

		++transformIndex;
	}

	materialsBuffer->SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_MATERIAL);
	transformsBuffer_->SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_TRANSFORM);


	/// 描画
	cmdList->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()),
		static_cast<UINT>(transformIndex),
		0, 0, 0
	);

}
