#include "TerrainRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Camera/Camera.h"

#include "Game/Objects/Terrain/Terrain.h"


TerrainRenderingPipeline::TerrainRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: pResourceCollection_(_resourceCollection) {}
TerrainRenderingPipeline::~TerrainRenderingPipeline() {}



void TerrainRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/Render/Terrain/Terrain.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/Terrain/Terrain.ps.hlsl", L"ps_6_0", Shader::Type::ps);


		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->SetShader(&shader);

		/// input element
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);


		/// buffer

		/// vs
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// ROOT_PARAM_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); /// ROOT_PARAM_TRANSFORM

		/// ps
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// grass
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// dirt
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// rock
		pipeline_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// snow

		for (uint32_t i = 0; i < 4; i++) {
			pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, i); /// textures
		}

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);



		/// setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);


		/// pipeline create
		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}


	{ /// buffer

		transformBuffer_.Create(_dxManager->GetDxDevice());

	}

}

void TerrainRenderingPipeline::Draw(DxCommand* _dxCommand, EntityComponentSystem* _entityComponentSystem, Camera* _camera) {


	/// 地形を取得
	if (pTerrain_ == nullptr) {

		for (auto& entity : _entityComponentSystem->GetEntities()) {

			if (entity->GetName() == "Terrain") {
				pTerrain_ = static_cast<Terrain*>(entity.get());
				break;
			}
		}

		/// 見つかんなかったらreturn
		if (pTerrain_ == nullptr) {
			return;
		}

	}


	/// value setting
	transformBuffer_.SetMappedData(pTerrain_->GetTransform()->matWorld);



	/// 描画する
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto command = _dxCommand->GetCommandList();

	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(command, ROOT_PARAM_VIEW_PROJECTION);
	transformBuffer_.BindForGraphicsCommandList(command, ROOT_PARAM_TRANSFORM);

	/// texs
	const auto& textures = pResourceCollection_->GetTextures();

	for (uint32_t i = 0; i < pTerrain_->GetSplatTexPaths().size(); i++) {
		const std::string& path = pTerrain_->GetSplatTexPaths()[i];
		size_t index = pResourceCollection_->GetTextureIndex(path);
		command->SetGraphicsRootDescriptorTable(
			static_cast<UINT>(ROOT_PARAM_TEX_GRASS + i),
			textures[index]->GetSRVGPUHandle()
		);
	}



	/// vbv ivb setting



}



