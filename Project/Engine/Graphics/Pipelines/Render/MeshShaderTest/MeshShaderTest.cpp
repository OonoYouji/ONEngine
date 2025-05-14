#include "MeshShaderTest.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/Entity/Camera/Camera.h"


void MeshShaderTest::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	{

		Shader shader;
		shader.Initialize(_shaderCompiler);

		//shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"Packages/Shader/Render/MeshShaderTest/MeshShaderTest.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();

		pipeline_->SetShader(&shader);

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		/// buffer
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< vertices
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< indices
		pipeline_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);  ///< meshlet

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 1 - 1);       ///< vertices
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 2 - 1);       ///< indices
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_MESH, 2);       ///< meshlet

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); ///< buffer length
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_MESH, 1); ///< view projection


		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{
		vertexBuffer_.Create(1000 * 1000, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		indexBuffer_.Create(999 * 999 * 6 / 3, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		meshletBuffer_.Create(30000, _dxManager->GetDxDevice(), _dxManager->GetDxSRVHeap());
		bufferLength_.Create(_dxManager->GetDxDevice());



	}

}

void MeshShaderTest::Draw(DxCommand* _dxCommand, [[maybe_unused]] EntityComponentSystem* _pEntityComponentSystem, [[maybe_unused]] Camera* _camera) {

	Terrain* terrain = nullptr;
	for (auto& entity : _pEntityComponentSystem->GetEntities()) {
		if (entity->GetName() == "Terrain") {
			terrain = static_cast<Terrain*>(entity.get());
			break;
		}
	}

	/// 地形が存在しないなら描画しない
	if (terrain == nullptr) {
		return;
	}

	/// meshletの設定
	for (size_t i = 0; i < terrain->GetMeshlets().size() - 1; i++) {
		meshletBuffer_.SetMappedData(
			i,
			{
				terrain->GetMeshlets()[i].meshlet
			}
		);

		if (i == 0) {
			/// meshletの最初の頂点と最後の頂点を得る

			auto& meshlet = terrain->GetMeshlets()[i];

			Vec4 firstPos = terrain->GetVertices()[meshlet.meshlet.vertex_offset].position;
			Vec4 endPos = terrain->GetVertices()[meshlet.meshlet.vertex_offset + meshlet.meshlet.vertex_count].position;

			Gizmo::DrawWireSphere(
				Vec3(firstPos.x, firstPos.y, firstPos.z), 0.5f, Color::kRed
			);
			
			Gizmo::DrawWireSphere(
				Vec3(endPos.x, endPos.y, endPos.z), 0.5f, Color::kGreen
			);

		}

	}

	static bool isSetting = false;
	if (!isSetting) {

		isSetting = true;

		/// 頂点の設定
		for (size_t i = 0; i < terrain->GetVertices().size() - 1; ++i) {
			const Mesh::VertexData& v = terrain->GetVertices()[i];
			vertexBuffer_.SetMappedData(
				i,
				{
					v.position, v.normal, v.uv
				}
			);
		}


		/// インデックスの設定
		for (size_t i = 0; i < terrain->GetIndices().size(); i += 3) {
			indexBuffer_.SetMappedData(
				i / 3,
				{
					terrain->GetIndices()[i + 0],
					terrain->GetIndices()[i + 1],
					terrain->GetIndices()[i + 2]
				}
			);
		}
	}

	bufferLength_.SetMappedData(
		{ static_cast<uint32_t>(terrain->GetVertices().size()),
		static_cast<uint32_t>(terrain->GetIndices().size() / 3) }
	);



	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto command = _dxCommand->GetCommandList();

	//meshletBuffer_.BindToCommandList(0, command);
	vertexBuffer_.BindToCommandList(0, command); /// vertices
	indexBuffer_.BindToCommandList(1, command);  /// indices
	meshletBuffer_.BindToCommandList(2, command); /// meshlet
	bufferLength_.BindForGraphicsCommandList(command, 4); /// buffer length
	_camera->GetViewProjectionBuffer()->BindForGraphicsCommandList(
		command, 4
	);


	command->DispatchMesh(32, 1, 1);
}
