#include "SkyboxRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


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

		pipeline_->AddDescriptorRange(0, 128, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
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

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{	/// buffer

		texIndex_.Create(_dxManager->GetDxDevice());
		transformMatrix_.Create(_dxManager->GetDxDevice());

		for (int x = -1; x <= 1; x += 2) {
			for (int y = -1; y <= 1; y += 2) {
				for (int z = -1; z <= 1; z += 2) {
					vertices_.push_back(VSInput(Vector4(
						static_cast<float>(x),
						static_cast<float>(y),
						static_cast<float>(z),
						1.0f
					)));
				}
			}
		}

		// インデックス（CW）
		indices_.insert(indices_.end(), { 0, 2, 3, 0, 3, 1 }); // -X
		indices_.insert(indices_.end(), { 4, 5, 7, 4, 7, 6 }); // +X
		indices_.insert(indices_.end(), { 0, 1, 5, 0, 5, 4 }); // -Y
		indices_.insert(indices_.end(), { 2, 6, 7, 2, 7, 3 }); // +Y
		indices_.insert(indices_.end(), { 0, 4, 6, 0, 6, 2 }); // -Z
		indices_.insert(indices_.end(), { 1, 3, 7, 1, 7, 5 }); // +Z




		const size_t kVertexDataSize = sizeof(VSInput);

		/// vertex buffer
		vertexBuffer_.CreateResource(_dxManager->GetDxDevice(), kVertexDataSize * vertices_.size());

		vbv_.BufferLocation = vertexBuffer_.Get()->GetGPUVirtualAddress();
		vbv_.SizeInBytes = static_cast<UINT>(kVertexDataSize * vertices_.size());
		vbv_.StrideInBytes = static_cast<UINT>(kVertexDataSize);

		/// mapping
		VSInput* mappingVertexData = nullptr;
		vertexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingVertexData));
		std::memcpy(mappingVertexData, vertices_.data(), sizeof(VSInput) * vertices_.size());
		vertexBuffer_.Get()->Unmap(0, nullptr);

		/// index buffer
		indexBuffer_.CreateResource(_dxManager->GetDxDevice(), sizeof(uint32_t) * indices_.size());

		ibv_.BufferLocation = indexBuffer_.Get()->GetGPUVirtualAddress();
		ibv_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices_.size());
		ibv_.Format = DXGI_FORMAT_R32_UINT;

		/// mapping
		uint32_t* mappingData = nullptr;
		indexBuffer_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappingData));
		std::memcpy(mappingData, indices_.data(), sizeof(uint32_t) * indices_.size());
		indexBuffer_.Get()->Unmap(0, nullptr);


	}
}

void SkyboxRenderingPipeline::Draw(class EntityComponentSystem* _ecs, const std::vector<IEntity*>&, CameraComponent* _camera, DxCommand* _dxCommand) {

	Skybox* pSkybox = nullptr;
	ComponentArray<Skybox>* skyboxArray = _ecs->GetComponentArray<Skybox>();
	if (!skyboxArray) {
		return;
	}

	for (auto& skybox : skyboxArray->GetUsedComponents()) {
		if (skybox && skybox->GetOwner()) {
			pSkybox = skybox;
			break;
		}
	}


	if (!pSkybox) {
		return;
	}

	//pSkybox->GetOwner()->UpdateTransform();

	auto& textures = pResourceCollection_->GetTextures();
	size_t texIndex = pResourceCollection_->GetTextureIndex(pSkybox->GetDDSTexturePath());

	texIndex_.SetMappedData(texIndex);
	transformMatrix_.SetMappedData(pSkybox->GetOwner()->GetTransform()->GetMatWorld());


	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto commandList = _dxCommand->GetCommandList();

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbv_);
	commandList->IASetIndexBuffer(&ibv_);

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(commandList, 0);
	transformMatrix_.BindForGraphicsCommandList(commandList, 1);
	texIndex_.BindForGraphicsCommandList(commandList, 2);
	commandList->SetGraphicsRootDescriptorTable(3, (*textures.begin())->GetSRVGPUHandle());

	commandList->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()),
		1, 0, 0, 0
	);


}
