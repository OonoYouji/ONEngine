#include "ModelLoader.h"

/// std
#include <fstream>

/// externals
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Tools/StringHash.h"
#include "Engine/Asset/Meta/MetaFile.h"


namespace ONEngine::Asset {

AssetLoader<Model>::AssetLoader(DxManager* _dxm)
	: pDxManager_(_dxm) {
	assimpLoadFlags_ = aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_LimitBoneWeights;
}

std::optional<Model> AssetLoader<Model>::Load(const std::string& _filepath, Meta<Model::MetaData> meta) {
	/// ----- モデルの読み込み ----- ///

	const std::string fileExtension = FileSystem::FileExtension(_filepath);
	Console::LogInfo(std::format("[Load] [Model] - Starting load: \"{}\" (ext: \"{}\")", _filepath, fileExtension));

	Assimp::Importer importer;
	const aiScene* scene = nullptr;
	
	try {
		scene = importer.ReadFile(_filepath, assimpLoadFlags_);
	} catch (const std::exception& e) {
		Console::LogError(std::format("[Load] [Model] - Assimp Exception: \"{}\", path: \"{}\"", e.what(), _filepath));
		return std::nullopt;
	} catch (...) {
		Console::LogError(std::format("[Load] [Model] - Unknown Assimp Exception, path: \"{}\"", _filepath));
		return std::nullopt;
	}

	/// 読み込めるモデルであるのかチェックする
	if(!ValidateModel(scene)) {
		if(!scene) {
			Console::LogError(std::format("[Load] [Model] - Assimp Error: \"{}\", path: \"{}\"", importer.GetErrorString(), _filepath));
		}
		return std::nullopt;
	}

	if(!scene) {
		return std::nullopt;
	}

	Model model;
	model.guid = meta.base.guid;
	model.SetPath(_filepath);

	/// mesh 解析
	model.GetMeshJointWeightData().resize(scene->mNumMeshes);

	for(uint32_t meshIndex = 0u; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		Console::LogInfo(std::format("  Mesh[{}]: \"{}\", Vertices: {}, Bones: {}", 
			meshIndex, mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumBones));

		/// sceneのデータを使ってMeshを作成する
		std::vector<Model::Vertex> vertices;
		std::vector<uint32_t>      indices;

		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3);

		/// vertex 解析
		for(uint32_t i = 0; i < mesh->mNumVertices; ++i) {
			Model::Vertex&& vertex = {
				Vector4(-mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f),
				mesh->HasTextureCoords(0) ? Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : Vector2::Zero,
				mesh->HasNormals() ? Vector3(-mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : Vector3::Up
			};

			vertices.push_back(vertex);
		}


		/// index 解析
		for(uint32_t i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for(uint32_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}


		/// joint 解析
		for(uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {

			/// 格納領域の作成
			aiBone* bone = mesh->mBones[boneIndex];
			std::string      jointName = bone->mName.C_Str();
			uint32_t         jointNameHash = StringHash::Get(jointName);
			JointWeightData& jointWeightData = model.GetMeshJointWeightData()[meshIndex][jointNameHash];

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
			for(uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back(
					{ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId }
				);
			}

		}

		if(fileExtension == ".gltf") {
			if(meshIndex == 0) {
				/// nodeの解析 (ルート一回のみ)
				model.SetRootNode(ReadNode(scene->mRootNode));
				LoadAnimation(&model, scene);
			}
		}

		/// mesh dataを作成
		std::unique_ptr<Model::ModelMesh> meshData = std::make_unique<Model::ModelMesh>();
		meshData->SetVertices(vertices);
		meshData->SetIndices(indices);

		/// bufferの作成
		meshData->CreateBuffer(pDxManager_->GetDxDevice());

		model.AddMesh(std::move(meshData));
	}

	Console::LogInfo(std::format("[Load] [Model] - Finished: \"{}\", Total Meshes: {}, Total Clips: {}", 
		_filepath, scene->mNumMeshes, model.GetAnimationClips().size()));

	return model;
}

std::optional<Model> AssetLoader<Model>::Reload(const std::string& _filepath, Model* /*_src*/, Meta<Model::MetaData> meta) {
	/// モデルの再読み込みは特殊な操作をする必要がないのでもう一度読み込んだ内容を渡す
	return Load(_filepath, meta);
}


Meta<Model::MetaData> AssetLoader<Model>::GetMetaData(const std::string& _filepath) {
	Meta<Model::MetaData> res{};

	const std::string metaPath = _filepath + ".meta";
	res.base = LoadOrGenerateMetaBase(metaPath, _filepath);

	nlohmann::json j;
	std::ifstream ifs(metaPath);
	if(!ifs.is_open()) {
		return {};
	}

	ifs >> j;
	Model::MetaData data;
	data.scale = j.value("scale", 1.0f);

	res.data = data;

	return res;
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
	for(size_t childIndex = 0; childIndex < _node->mNumChildren; ++childIndex) {
		result.children[childIndex] = ReadNode(_node->mChildren[childIndex]);
	}

	return result;
}

void AssetLoader<Model>::LoadAnimation(Model* _model, const aiScene* _scene) {
	/// ----- アニメーションの読み込み ----- ///

	///!< アニメーションが存在しない場合は何もしない
	if(!_scene->mAnimations || _scene->mNumAnimations == 0) {
		return;
	}

	for(uint32_t animIndex = 0; animIndex < _scene->mNumAnimations; ++animIndex) {
		aiAnimation* animationAssimp = _scene->mAnimations[animIndex];
		
		std::string clipName = animationAssimp->mName.C_Str();
		if(clipName.empty()) {
			clipName = "Animation_" + std::to_string(animIndex);
		}

		uint32_t clipNameHash = StringHash::Get(clipName);
		AnimationClip& clip = _model->GetAnimationClips()[clipNameHash];
		clip.name = clipName;
		clip.nameHash = clipNameHash;
		clip.duration = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

		Console::Log(std::format("[Load] [AnimationClip] - name:\"{}\", hash: {}, duration: {:.2f}s", clipName, clip.nameHash, clip.duration));

		/// node animationの読み込み
		for(uint32_t channelIndex = 0u; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

			/// node animationの解析用データを
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			std::string nodeName = nodeAnimationAssimp->mNodeName.C_Str();
			uint32_t nodeNameHash = StringHash::Get(nodeName);
			NodeAnimation& nodeAnimation = clip.nodeAnimationMap[nodeNameHash];

			/// ---------------------------------------------------
			/// translateの解析
			/// ---------------------------------------------------
			for(uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {

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
			for(uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {

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
			for(uint32_t keyIndex = 0u; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {

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
}

bool AssetLoader<Model>::ValidateModel(const aiScene* _aiScene) {
	if(!_aiScene) {
		Console::LogError("AssetLoader<Model>::ValidateModel: aiScene is null.");
		return false;
	}
	if(!_aiScene->mNumMeshes) {
		Console::LogError("AssetLoader<Model>::ValidateModel: Model has no meshes.");
		return false;
	}

	for(unsigned int i = 0; i < _aiScene->mNumMeshes; i++) {
		const aiMesh* mesh = _aiScene->mMeshes[i];
		if(!mesh) {
			Console::LogError(std::format("AssetLoader<Model>::ValidateModel: Mesh[{}] is null.", i));
			return false;
		}

		/// UV 
		bool hasUV = (mesh->mTextureCoords[0] != nullptr);

		/// 法線 
		bool hasNormals = mesh->HasNormals();

		/// 三角形チェック
		bool isTriangulated = true;
		for(unsigned int f = 0; f < mesh->mNumFaces; f++) {
			constexpr uint32_t triangleIndices = 3;
			if(mesh->mFaces[f].mNumIndices != triangleIndices) {
				isTriangulated = false;
				break;
			}
		}

		/// 1つでも条件を満たさないメッシュがあれば警告を出し、デフォルト値で対応する
		if(!hasUV || !hasNormals || !isTriangulated) {
			Console::LogWarning(std::format("AssetLoader<Model>::ValidateModel: Mesh[{}] (\"{}\") lacks some data (UV:{}, Normal:{}, Tri:{}) - Using defaults.", 
				i, mesh->mName.C_Str(), hasUV, hasNormals, isTriangulated));
			
			// 三角形化されていない場合のみ、法線やUVがない場合より致命的なので、一応続行するがAssimpのフラグでカバーされているはず
			if (!isTriangulated) {
				// return false; // AssimpのaiProcess_Triangulateがあるので、基本ここには来ないはず
			}
		}
	}

	return true;
}


} /// namespace ONEngine::Asset