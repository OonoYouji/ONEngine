#include "ModelManager.h"

/// std
#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>

/// externals
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <Core/ONEngine.h>

#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommand.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxShaderCompiler.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"

#include "Objects/Camera/Manager/CameraManager.h"
#include "GraphicManager/TextureManager/TextureManager.h"
#include "GraphicManager/Light/DirectionalLight.h"

#include "Debugger/Assertion.h"


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
		pipeline->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- transform
		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);		///- material
		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);		///- directional light
		pipeline->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 2);		///- camera 

		pipeline->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
		pipeline->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

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
/// モデルの読み込み
/// ===================================================
Model* ModelManager::Load(const std::string& filePath) {

	ModelManager* instance = GetInstance();
	auto itr = instance->models_.find(filePath);
	if(itr != instance->models_.end()) {
		return (*itr).second.get();
	}


	Assimp::Importer importer;
	std::string objPath = instance->kDirectoryPath_ + filePath + "/" + filePath + ".obj";
	const aiScene* scene = importer.ReadFile(objPath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);
	if(scene == nullptr) {
		objPath = instance->kDirectoryPath_ + filePath + "/" + filePath + ".gltf";
		scene = importer.ReadFile(objPath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	}

	Model* model = new Model();

	/// ---------------------------------------------------
	/// mesh解析
	/// ---------------------------------------------------
	for(uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		Assert(mesh->HasNormals(), "not has normals");
		Assert(mesh->HasTextureCoords(0), "not has texcoord");

		Mesh modelMesh;

		/// ---------------------------------------------------
		/// vertex解析
		/// ---------------------------------------------------
		for(uint32_t i = 0; i < mesh->mNumVertices; ++i) {
			aiVector3D& position = mesh->mVertices[i];
			aiVector3D& normal = mesh->mNormals[i];
			aiVector3D& texcoord = mesh->mTextureCoords[0][i];

			Mesh::VertexData vertex;
			vertex.position = { -position.x, position.y, position.z, 1.0f };
			vertex.texcoord = { texcoord.x, texcoord.y };
			vertex.normal = { -normal.x, normal.y, normal.z };


			modelMesh.AddVertex(vertex);
		}

		/// ---------------------------------------------------
		/// index解析
		/// ---------------------------------------------------
		for(uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			Assert(face.mNumIndices == 3, "not triangles");

			for(uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				uint32_t& index = face.mIndices[element];

				modelMesh.AddIndex(index);
			}

		}

		/// ---------------------------------------------------
		/// joint解析
		/// ---------------------------------------------------
		for(uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

			/// 格納領域の作成
			aiBone*          bone            = mesh->mBones[boneIndex];
			std::string      jointName       = bone->mName.C_Str();
			JointWeightData& jointWeightData = model->skinClusterData_[jointName];

			/// mat bind pose inverseの計算
			aiMatrix4x4  matBindPoseAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D   position;
			aiQuaternion rotate;
			aiVector3D   scale;

			matBindPoseAssimp.Decompose(scale, rotate, position);
			Mat4 matBindPose = 
				Mat4::MakeScale({ scale.x, scale.y, scale.z })
				* Mat4::MakeRotateQuaternion(Quaternion::Normalize({ rotate.x, -rotate.y, -rotate.z, rotate.w }))
				* Mat4::MakeTranslate({ -position.x, position.y, position.z });

			jointWeightData.matBindPoseInverse = matBindPose.Inverse();


			/// weight情報を取り出す
			for(uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back(
					{ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId }
				);
			}

		}

		modelMesh.CreateBuffer();
		model->AddMesh(modelMesh);

	}


	/// ---------------------------------------------------
	/// material解析
	/// ---------------------------------------------------
	for(uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		uint32_t texCount = material->GetTextureCount(aiTextureType_DIFFUSE);
		if(!texCount) {
			continue;
		}

		aiString texFilePath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texFilePath);
		Material modelMaterial;
		modelMaterial.SetFilePath(texFilePath.C_Str());

		std::filesystem::path path(texFilePath.C_Str());
		std::string texPath = path.filename().string();
		std::string::size_type dotIndex = texPath.rfind('.');
		if(dotIndex != std::string::npos) {
			texPath = texPath.substr(0, dotIndex);
		}
		modelMaterial.SetTextureName(texPath);

		TextureManager::GetInstance()->Load(
			texPath, 
			path.filename().string()
		);

		modelMaterial.CreateBuffer();
		model->AddMaterial(modelMaterial);

	}

	Node rootNode = instance->ReadNode(scene->mRootNode);
	model->SetRootNode(rootNode);

	if(model->GetMaterials().empty()) {
		Material material;
		material.CreateMaterial("white2x2");
		model->AddMaterial(material);
	}


	model->Initialize();
	model->SetFillMode(kSolid);
	instance->AddModel(filePath, model);
	return GetModel(filePath);
}


Node ModelManager::ReadNode(aiNode* node) {
	Node result;

	aiVector3D   position;
	aiQuaternion rotate;
	aiVector3D   scale;

	node->mTransformation.Decompose(scale, rotate, position);

	result.transform.scale      = { scale.x, scale.y, scale.z };
	result.transform.quaternion = { rotate.x, -rotate.y, -rotate.z, rotate.w };
	result.transform.position   = { -position.x, position.y, position.z };
	result.transform.Update();

	/// nodeから必要な値をゲット
	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);

	/// childrenの解析
	for(size_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}


/// ===================================================
/// 平面の生成
/// ===================================================
Model* ModelManager::CreatePlane() {

	ModelManager* instance = GetInstance();
	auto itr = instance->models_.find("Plane");
	if(itr != instance->models_.end()) {
		return (*itr).second.get();
	}

	Model* model = new Model();

	Mesh mesh{};
	Material material{};

	mesh.AddVertex({ { -1.0f, 0.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh.AddVertex({ { -1.0f, 0.0f,  1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh.AddVertex({ {  1.0f, 0.0f,  1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } });
	mesh.AddVertex({ {  1.0f, 0.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } });

	mesh.AddIndex(0);
	mesh.AddIndex(1);
	mesh.AddIndex(2);
	mesh.AddIndex(0);
	mesh.AddIndex(2);
	mesh.AddIndex(3);

	material.SetTextureName("uvChecker");
	material.SetFilePath("uvChecker.png");

	mesh.CreateBuffer();
	material.CreateBuffer();

	model->AddMesh(mesh);
	model->AddMaterial(material);

	model->Initialize();
	model->SetFillMode(kSolid);
	instance->AddModel("Plane", model);
	return instance->GetModel("Plane");
}


/// ===================================================
/// 立方体の生成
/// ===================================================
Model* ModelManager::CreateCube() {

	ModelManager* instance = GetInstance();
	auto itr = instance->models_.find("Cube");
	if(itr != instance->models_.end()) {
		return (*itr).second.get();
	}

	Model* model = new Model();

	Mesh mesh{};
	Material material{};

	///- 上面
	mesh.AddVertex({ { -1.0f,  1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f} });
	mesh.AddVertex({ { -1.0f,  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f }, {  0.0f,  1.0f,  0.0f} });
	mesh.AddVertex({ {  1.0f,  1.0f,  1.0f, 1.0f }, { 1.0f, 0.0f }, {  0.0f,  1.0f,  0.0f} });
	mesh.AddVertex({ {  1.0f,  1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f} });

	///- 下面
	mesh.AddVertex({ {  1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }, {  0.0f, -1.0f,  0.0f} });
	mesh.AddVertex({ {  1.0f, -1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f }, {  0.0f, -1.0f,  0.0f} });
	mesh.AddVertex({ { -1.0f, -1.0f,  1.0f, 1.0f }, { 1.0f, 0.0f }, {  0.0f, -1.0f,  0.0f} });
	mesh.AddVertex({ { -1.0f, -1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f} });

	///- 前面
	mesh.AddVertex({ { -1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f, -1.0f } });
	mesh.AddVertex({ { -1.0f,  1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } });
	mesh.AddVertex({ {  1.0f,  1.0f, -1.0f, 1.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } });
	mesh.AddVertex({ {  1.0f, -1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f, -1.0f } });

	///- 後面
	mesh.AddVertex({ {  1.0f, -1.0f,  1.0f, 1.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } });
	mesh.AddVertex({ {  1.0f,  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } });
	mesh.AddVertex({ { -1.0f,  1.0f,  1.0f, 1.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } });
	mesh.AddVertex({ { -1.0f, -1.0f,  1.0f, 1.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } });

	///- 左面
	mesh.AddVertex({ { -1.0f, -1.0f,  1.0f, 1.0f }, { 0.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } });
	mesh.AddVertex({ { -1.0f,  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } });
	mesh.AddVertex({ { -1.0f,  1.0f, -1.0f, 1.0f }, { 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } });
	mesh.AddVertex({ { -1.0f, -1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } });

	///- 右面
	mesh.AddVertex({ {  1.0f, -1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f} });
	mesh.AddVertex({ {  1.0f,  1.0f, -1.0f, 1.0f }, { 0.0f, 0.0f }, { -1.0f,  0.0f,  0.0f} });
	mesh.AddVertex({ {  1.0f,  1.0f,  1.0f, 1.0f }, { 1.0f, 0.0f }, { -1.0f,  0.0f,  0.0f} });
	mesh.AddVertex({ {  1.0f, -1.0f,  1.0f, 1.0f }, { 1.0f, 1.0f }, { -1.0f,  0.0f,  0.0f} });


	for(uint32_t i = 0u; i < 6; ++i) {
		mesh.AddIndex(0 + (i * 4));
		mesh.AddIndex(1 + (i * 4));
		mesh.AddIndex(2 + (i * 4));
		mesh.AddIndex(0 + (i * 4));
		mesh.AddIndex(2 + (i * 4));
		mesh.AddIndex(3 + (i * 4));
	}

	material.SetTextureName("uvChecker");
	material.SetFilePath("uvChecker.png");

	mesh.CreateBuffer();
	material.CreateBuffer();

	model->AddMesh(mesh);
	model->AddMaterial(material);

	model->Initialize();
	model->SetFillMode(kSolid);
	instance->AddModel("Cube", model);
	return instance->GetModel("Cube");
}


/// ===================================================
/// 描画前処理
/// ===================================================
void ModelManager::PreDraw() {
	activeModels_.clear();
}


/// ===================================================
/// 描画後処理
/// ===================================================
void ModelManager::PostDraw() {

	std::list<Element> solid;
	std::list<Element> wire;

	/// ---------------------------------------------------
	/// SolidとWireFrameで仕分け
	/// ---------------------------------------------------
	for(const auto& model : activeModels_) {
		if(model.fillMode == FillMode::kSolid) {
			solid.push_back(model);
		} else {
			wire.push_back(model);
		}
	}


	ID3D12GraphicsCommandList* commandList    = ONEngine::GetDxCommon()->GetDxCommand()->GetList();
	CameraManager*             pCameraManager = CameraManager::GetInstance();
	BaseCamera*                pCamera        = pCameraManager->GetMainCamera();


	/// ---------------------------------------------------
	/// Solidの描画
	/// ---------------------------------------------------

	pipelines_[kSolid]->SetPipelineState();

	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootConstantBufferView(0, pCamera->GetViewBuffer()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(4, pCamera->GetPositionBuffer()->GetGPUVirtualAddress());
	pDirectionalLight_->BindToCommandList(3, commandList);

	for(auto& model : solid) {
		model.transform->BindTransform(commandList, 1, model.matLocal);
		model.model->DrawCall(commandList, model.material, 2, 5);
	}


	/// ---------------------------------------------------
	/// WireFrameの描画
	/// ---------------------------------------------------

	pipelines_[kWireFrame]->SetPipelineState();

	for(auto& model : wire) {
		model.transform->BindTransform(commandList, 1, model.matLocal);
		model.model->DrawCall(commandList, model.material, 2, 5);
	}


}

Model* ModelManager::GetModel(const std::string& filePath) {
	return GetInstance()->models_.at(filePath).get();
}


/// ===================================================
/// Modelの追加
/// ===================================================
void ModelManager::AddModel(const std::string& name, Model* model) {
	std::unique_ptr<Model> add(model);
	models_[name] = std::move(add);
}


/// ===================================================
/// pipelineのセット
/// ===================================================
void ModelManager::SetPipelineState(FillMode fillMode) {
	pipelines_[fillMode]->SetPipelineState();
}


/// ===================================================
/// アクティブなモデルの追加
/// ===================================================
void ModelManager::AddActiveModel(Model* model, Transform* transform, Mat4* matLocal, Material* material, FillMode fillMode) {
	Element element{};
	element.model     = model;
	element.transform = transform;
	element.material  = material;
	element.fillMode  = fillMode;
	element.matLocal  = matLocal;

	activeModels_.push_back(element);
}

void ModelManager::SetDirectionalLight(DirectionalLight* directionalLight) {
	pDirectionalLight_ = directionalLight;
}
