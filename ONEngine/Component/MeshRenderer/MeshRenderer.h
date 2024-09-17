#pragma once

#include <memory>

#include <Component/Base/BaseComponent.h>
#include <Model.h>


/// ===================================================
/// モデルの描画を行うクラス
/// ===================================================
class MeshRenderer final : public BaseComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	MeshRenderer() {}
	~MeshRenderer() {}

	void Initialize() override;
	void Draw() override;


	/// <summary>
	/// モデルのセット
	/// </summary>
	void SetModel(const std::string& filePath);
	void SetModel(Model* model);

	/// <summary>
	/// マテリアルのセット
	/// </summary>
	/// <param name="texName">: "./Resources/Textures/" からのfilePath</param>
	void SetMaterial(const std::string& texName);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	Model*						model_		= nullptr;
	Transform*					pTransform_	= nullptr;
	FillMode					fillMode_	= kSolid;
	std::unique_ptr<Material>	material_	= nullptr;
};