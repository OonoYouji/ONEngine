#pragma once

#include <GameObjectManager.h>


/// <summary>
/// 潜水艦
/// </summary>
class Submarine final : public BaseGameObject {
public:

	Submarine() {
		CreateTag(this);
	}
	~Submarine() {}


	void Initialize() override;
	void Update() override;

	void Draw() override;

	void Debug() override;

private:

	Model* model_ = nullptr;

	/// エレベータの設置の始点、終点
	Vec3 startPos_, endPos_;
	/// マウスのクリックし始め、終わりのフラグ
	bool isEnter_, isExit_;
	bool isMouseToCollision_;

	/// カメラとの距離
	float toCameraDistance_ = 0.0f;

	/// マウスの操作で作成するワイヤー
	class Wire* newWire_ = nullptr;

};