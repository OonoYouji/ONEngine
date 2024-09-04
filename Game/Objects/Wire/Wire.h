#pragma once

#include <GameObjectManager.h>


/// <summary>
/// 潜水艦から伸びるワイヤー
/// </summary>
class Wire final : public BaseGameObject {
public:

	Wire() {
		CreateTag(this);
	}
	~Wire() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

	void Debug() override;


	/// ---------------------------------------- ///
	/// not override methods
	/// ---------------------------------------- ///

	const Vec3& GetTop() const { return top_; }
	void SetTop(const Vec3& top) { top_ = top; }

	const Vec3& GetBottom() const { return bottom_; }
	void SetBottom(const Vec3& bottom) { bottom_ = bottom; }

private:

	Model* model_ = nullptr;


	/// ワイヤーの一番上と一番下の座標
	Vec3 top_, bottom_;


};