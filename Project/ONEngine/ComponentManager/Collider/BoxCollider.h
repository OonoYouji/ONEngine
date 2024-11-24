#pragma once

#include <array>

#include "BaseCollider.h"

/// ===================================================
/// ボックス コライダーのクラス
/// ===================================================
class BoxCollider final : public BaseCollider {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	BoxCollider(const Model* model) {
		CreateCollider(model);
	}
	~BoxCollider() {}

	void Initialize()	override;
	void Update()		override;
	void Draw()			override;

	/// <summary>
	/// コライダーの作成
	/// </summary>
	/// <param name="model">: modelを基にOBBを作成する</param>
	void CreateCollider(const Model* model) override;


	std::vector<Vec3> GetVertices() const;
	const std::array<Vec3, 3>& GetOrientations() const {
		return orientatinos_;
	}


	void SetSize(const Vec3& _size);

	const Vec3& GetSize() const {
		return size_;
	}

	const Mat4& GetObbMatTransform() const {
		return obbMatTransform_;
	}


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// <summary>
	/// orientations_を計算する
	/// </summary>
	void CalculationOrientations();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Vec3				size_				= {};
	std::array<Vec3, 3> orientatinos_		= {};
	Model*				cube_				= nullptr;
	Mat4				obbMatTransform_	= Mat4::kIdentity;
};