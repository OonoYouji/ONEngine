#pragma once

#include "GameObjectManager/BaseGameObject.h"

/// ===================================================
/// プレイヤーのモデルを描画する専用クラス
/// ===================================================
class PlayerMesh : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	PlayerMesh();
	~PlayerMesh();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class MeshRenderer* meshRenderer_ = nullptr;

	Vec3 offsetPosition_ = { 0.0f, 0.0f, 0.0f };

	/// animationの時間、速度、幅
	float animationTime_  = 0.0f;
	float animationSpeed_ = 1.0f;
	float animationRange_ = 1.0f;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	Model* GetModel() const;

};
