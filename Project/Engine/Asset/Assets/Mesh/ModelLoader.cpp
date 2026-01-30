#include "ModelLoader.h"

/// externals
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"

using namespace ONEngine;


AssetLoader<Model>::AssetLoader(DxManager* _dxm)
	: pDxManager_(_dxm) {
	assimpLoadFlags_ = aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
}

std::optional<Model> AssetLoader<Model>::Load(const std::string& _filepath) {
	/// ----- モデルの読み込み ----- ///
	/// ファイルの拡張子を取得
	const std::string fileExtension = FileSystem::FileExtension(_filepath);
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_filepath, assimpLoadFlags_);

	/// 読み込めるモデルであるのかチェックする
	if (!ValidateModel(scene)) {
		return std::nullopt;
	}

	if (!scene) {
		return std::nullopt;
	}

	Model model;
	model.SetPath(_filepath);

	/// mesh 解析
	for (uint32_t meshIndex = 0u; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		/// sceneのデータを使ってMeshを作成する
		std::vector<Model::Vertex> vertices;
		std::vector<uint32_t>      indices;

		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);

		/// vertex 解析
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
			Model::Vertex&& vertex = {
				Vector4(-mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f),
				Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y),
				Vector3(-mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
			};

			vertices.push_back(vertex);
		}


		/// index 解析
		for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}


		/// joint 解析
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

			/// 格納領域の作成
			aiBone* bone = mesh->mBones[boneIndex];
			std::string      jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = model.GetJointWeightData()[jointName];

			/// mat bind pose inverseの計算
			aiMatrix4x4  matBindPoseAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D   position;
			aiQuaternion rotate;
			aiVector3D   scale;

			matBindPoseAssimp.Decompose(scale, rotate, position);
			Matrix4x4 matBindPose =
				Matrix4x4::MakeScale({ scale.x, scale.y, scale.z })
				* Matrix4x4::MakeRotate(Quaternion::Normalize({ rotate.x, -rotate.y, -rotate.z, rotate.w }))
				* Matrix4x4::MakeTranslate({ -position.x, position.y, position.z });

			jointWeightData.matBindPoseInverse = matBindPose.Inverse();


			/// weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back(
					{ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId }
				);
			}

		}

		if (fileExtension == ".gltf") {
			/// nodeの解析
			model.SetRootNode(ReadNode(scene->mRootNode));
			LoadAnimation(&model, _filepath);
		}

		/// mesh dataを作成
		std::unique_ptr<Model::ModelMesh> meshData = std::make_unique<Model::ModelMesh>();
		meshData->SetVertices(vertices);
		meshData->SetIndices(indices);

		/// bufferの作成
		meshData->CreateBuffer(pDxManager_->GetDxDevice());

		model.AddMesh(std::move(meshData));
	}

	Console::Log("[Load] [Model] - path:\"" + _filepath + "\"");

	return model;
}

std::optional<Model> AssetLoader<Model>::Reload(const std::string& _filepath, Model* /*_src*/) {

	/// モデルの再読み込みは特殊な操作をする必要がないのでもう一度読み込んだ内容を渡す
	return Load(_filepath);
}


Node AssetLoader<Model>::ReadNode(aiNode* _node) {
	/// ----- nodeの読み込み ----- ///

	Node result;

	aiVector3D   position;
	aiQuaternion rotate;
	aiVector3D   scale;

	_node->mTransformation.Decompose(scale, rotate, position);

	result.transform.scale = { scale.x, scale.y, scale.z };
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w };
	result.transform.position = { -position.x, position.y, position.z };
	result.transform.Update();

	/// nodeから必要な値をゲット
	result.name = _node->mName.C_Str();
	result.children.resize(_node->mNumChildren);

	/// childrenの解析
	for (size_t childIndex = 0; childIndex < _node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(_node->mChildren[childIndex]);
	}

	return result;
}

void AssetLoader<Model>::LoadAnimation(Model* _model, const std::string& _filepath) {
	/// ----- アニメーションの読み込み ----- ///

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_filepath.c_str(), 0);

	///!< アニメーションが存在しない場合は何もしない
	if (scene->mAnimations == 0) {
		Console::Log("[warning] type:Animation, path:\"" + _filepath + "\"");
		return; ///< アニメーションが存在しない場合は何もしない
	}

	/// 解析
	aiAnimation* animationAssimp = scene->mAnimations[0];

	float duration = _model->GetAnimationDuration();
	std::unordered_map<std::string, NodeAnimation>& nodeAnimationMap = _model->GetNodeAnimationMap();

	duration = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);
	_model->SetAnimationDuration(duration);

	/// node animationの読み込み
	for (uint32_t channelIndex = 0u; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

		/// node animationの解析用データを
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = nodeAnimationMap[nodeAnimationAssimp->mNodeName.C_Str()];

		/// ---------------------------------------------------
		/// translateの解析
		/// ---------------------------------------------------
		for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {

			/// keyの値を得る
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyFrameVector3 keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }
			};

			nodeAnimation.translate.push_back(keyframe);
		}


		/// ---------------------------------------------------
		/// rotateの解析
		/// ---------------------------------------------------
		for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {

			/// keyの値を得る
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyFrameQuaternion keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }
			};

			nodeAnimation.rotate.push_back(keyframe);
		}


		/// ---------------------------------------------------
		/// scaleの解析
		/// ---------------------------------------------------
		for (uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {

			/// keyの値を得る
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyFrameVector3 keyframe{
				.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond),
				.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }
			};

			nodeAnimation.scale.push_back(keyframe);
		}

	}
}

bool AssetLoader<Model>::ValidateModel(const aiScene* _aiScene) {
	if (!_aiScene || !_aiScene->mNumMeshes) {
		return false;
	}

	for (unsigned int i = 0; i < _aiScene->mNumMeshes; i++) {
		const aiMesh* mesh = _aiScene->mMeshes[i];
		if (!mesh) {
			return false;
		}

		/// UV 
		bool hasUV = (mesh->mTextureCoords[0] != nullptr);

		/// 法線 
		bool hasNormals = mesh->HasNormals();

		/// 三角形チェック
		bool isTriangulated = true;
		for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
			constexpr uint32_t triangleIndices = 3;
			if (mesh->mFaces[f].mNumIndices != triangleIndices) {
				isTriangulated = false;
				break;
			}
		}

		/// 1つでも条件を満たさないメッシュがあれば対象外 
		if (!hasUV || !hasNormals || !isTriangulated) {
			return false;
		}
	}

	return true;
}
