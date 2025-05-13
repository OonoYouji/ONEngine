#include "SkyboxRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/Entity/Skybox/Skybox.h"
#include "Engine/ECS/Entity/Camera/Camera.h"


SkyboxRenderingPipeline::SkyboxRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: pResourceCollection_(_resourceCollection) {}
SkyboxRenderingPipeline::~SkyboxRenderingPipeline() {}

void SkyboxRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/Render/Skybox/Skybox.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/Skybox/Skybox.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);


		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);

		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->SetBlendDesc(BlendMode::Normal());

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{	/// buffer

		texIndex_.Create(_dxManager->GetDxDevice());
		transformMatrix_.Create(_dxManager->GetDxDevice());


		/// vbv
		vertices_ = {
			/// 右面
			{ { -1.0f, -1.0f, 1.0f, 1.0f } },
			{ { 1.0f, -1.0f, 1.0f, 1.0f } },
			{ { -1.0f, 1.0f, 1.0f, 1.0f } },
			{ { 1.0f, 1.0f, 1.0f, 1.0f } },

			/// 左面
			{ { -1.0f, -1.0f, -1.0f, 1.0f } },
			{ { -1.0f, -1.0f, 1.0f, 1.0f } },
			{ { -1.0f, 1.0f, -1.0f, 1.0f } },
			{ { -1.0f, 1.0f, 1.0f, 1.0f } },

			/// 前面
			{ { -1.0f, -1.0f, -1.0f, 1.0f } },
			{ { 1.0f, -1.0f, -1.0f, 1.0f } },
			{ { -1.0f, 1.0f, -1.0f, 1.0f } },
			{ { 1.0f, 1.0f, -1.0f, 1.0f } },

			/// 後面
			{ { 1.0f, -1.0f, 1.0f, 1.0f } },
			{ { -1.0f, -1.0f, 1.0f, 1.0f } },
			{ { 1.0f, 1.0f, 1.0f, 1.0f } },
			{ { -1.0f, 1.0f, 1.0f, 1.0f } },

			/// 上面
			{ { -1.0f, 1.0f, -1.0f, 1.0f } },
			{ { 1.0f, 1.0f, -1.0f, 1.0f } },
			{ { -1.0f, 1.0f, 1.0f, 1.0f } },
			{ { 1.0f, 1.0f, 1.0f, 1.0f } },

			/// 下面
			{ { -1.0f, -1.0f, -1.0f, 1.0f } },
			{ { 1.0f, -1.0f, -1.0f, 1.0f } },
			{ { -1.0f, -1.0f, 1.0f, 1.0f } },
			{ { 1.0f, -1.0f, 1.0f, 1.0f } }
		};

		indices_ = {
			/// 右面
			0, 1, 2,
			2, 1, 3,
			/// 左面
			4, 5, 6,
			6, 5, 7,
			/// 前面
			8, 9, 10,
			10, 9, 11,
			/// 後面
			12, 13, 14,
			14, 13, 15,
			/// 上面
			16, 17, 18,
			18, 17, 19,
			/// 下面
			20, 21, 22,
			22, 21, 23
		};



		const size_t kVertexDataSize = sizeof(VSInput);

		/// vertex buffer
		vertexBuffer_.CreateResource(_dxManager->GetDxDevice(), kVertexDataSize * vertices_.size());

		vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
		vbv_.SizeInBytes = static_cast<UINT>(kVertexDataSize * vertices_.size());
		vbv_.StrideInBytes = static_cast<UINT>(kVertexDataSize);

		/// mapping
		VSInput* mappingVertexData = nullptr;
		vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingVertexData));
		std::memcpy(mappingVertexData, vertices_.data(), sizeof(VSInput)* vertices_.size());
		vertexBuffer_.Get()->Unmap(0, nullptr);

		/// index buffer
		indexBuffer_.CreateResource(_dxManager->GetDxDevice(), sizeof(uint32_t) * indices_.size());

		ibv_.BufferLocation = indexBuffer_.Get()->GetGPUVirtualAddress();
		ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
		ibv_.Format = DXGI_FORMAT_R32_UINT;

		/// mapping
		uint32_t* mappingData = nullptr;
		indexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData));
		std::memcpy(mappingData, indices_.data(), sizeof(uint32_t)* indices_.size());
		indexBuffer_.Get()->Unmap(0, nullptr);


	}
}

void SkyboxRenderingPipeline::Draw(DxCommand* _dxCommand, EntityComponentSystem* _pEntityComponentSystem, Camera* _camera) {


	Skybox* skybox = nullptr;
	for (auto& entity : _pEntityComponentSystem->GetEntities()) {
		if (entity->GetName() == "Skybox") {
			skybox = static_cast<Skybox*>(entity.get());
			break;
		}
	}

	if (!skybox) {
		return;
	}

	auto& textures = pResourceCollection_->GetTextures();
	size_t texIndex = pResourceCollection_->GetTextureIndex(skybox->GetTexturePath());

	texIndex_.SetMappingData(texIndex);
	transformMatrix_.SetMappingData(skybox->GetTransform()->GetMatWorld());


	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto commandList = _dxCommand->GetCommandList();

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);

	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(commandList, 0);
	transformMatrix_.BindForGraphicsCommandList(commandList, 1);
	texIndex_.BindForGraphicsCommandList(commandList, 2);
	commandList->SetGraphicsRootDescriptorTable(3, (*textures.begin())->GetSRVGPUHandle());

	commandList->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()),
		1,
		0, 0, 0
	);


}
