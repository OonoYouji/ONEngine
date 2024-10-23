#pragma once

#include <memory>

#include "ComponentManager/Base/BaseComponent.h"

#include "GraphicManager/ModelManager/Model.h"


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

	Material* GetMaterial() const { return material_.get(); }

	/// <summary>
	/// マテリアルの色のセット
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vec4& color);

	/// <summary>
	/// マテリアルの色のゲット
	/// </summary>
	/// <returns></returns>
	const Vec4& GetColor() const { return material_->GetColor(); }

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	Model*						model_		= nullptr;
	Transform*					pTransform_	= nullptr;
	FillMode					fillMode_	= kSolid;
	std::unique_ptr<Material>	material_	= nullptr;
};