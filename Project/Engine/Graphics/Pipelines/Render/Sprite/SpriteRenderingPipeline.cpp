#include "SpriteRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


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


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);                  ///< view projection : 0

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< transform
		pipeline_->AddDescriptorRange(1, 32, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< transform
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);       ///< textureID : 1
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);       ///< texture   : 2
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 2);      ///< transform : 3

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

		pipeline_->SetRTVNum(1);
		pipeline_->SetRTVFormats({ DXGI_FORMAT_R8G8B8A8_UNORM });

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

		textureIDsBuffer_ = std::make_unique<StructuredBuffer<uint32_t>>();
		textureIDsBuffer_->Create(static_cast<uint32_t>(kMaxRenderingSpriteCount_), _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());

	}


}

void SpriteRenderingPipeline::Draw(class EntityComponentSystem*, const std::vector<IEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// entityから sprite renderer を集める
	for (auto& entity : _entities) {
		SpriteRenderer*&& spriteRenderer = entity->GetComponent<SpriteRenderer>();
		if (spriteRenderer) {
			renderers_.push_back(spriteRenderer);
		}
	}

	///< rendererがなければ 早期リターン
	if (renderers_.empty()) {
		return;
	}



	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	//Camera2D* camera = nullptr; ///< TODO: 仮のカメラ取得
	//if (_pEntityComponentSystem->GetCamera2Ds().size() == 0) {
	//	return;
	//} else {
	//	camera = _pEntityComponentSystem->GetCamera2Ds()[0];
	//}

	/// settings
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	/// vbv, ivb setting
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(commandList, 0); ///< view projection

	/// 先頭の texture gpu handle をセットする
	auto& textures = resourceCollection_->GetTextures();
	commandList->SetGraphicsRootDescriptorTable(2, (*textures.begin())->GetSRVGPUHandle()); ///< texture


	/// bufferにデータをセット
	size_t transformIndex = 0;
	for (auto& renderer : renderers_) {

		size_t textureID = resourceCollection_->GetTextureIndex(renderer->GetTexturePath());
		textureIDsBuffer_->SetMappedData(
			transformIndex,
			textures[textureID]->GetSRVDescriptorIndex()
		);

		transformsBuffer_->SetMappedData(
			transformIndex,
			renderer->GetOwner()->GetTransform()->GetMatWorld()
		);

		++transformIndex;
	}

	textureIDsBuffer_->BindToCommandList(1, commandList); ///< textureID
	transformsBuffer_->BindToCommandList(3, commandList); ///< transform


	/// 描画
	commandList->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()),
		static_cast<UINT>(renderers_.size()),
		0, 0, 0
	);


	renderers_.clear();
}
