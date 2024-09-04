#pragma once

#include <memory>

#include <GameObjectManager.h>
#include <SpriteManager.h>


class Submarine;

/// <summary>
/// 潜水艦上に出てくるマウスの誘導
/// </summary>
class Guide final : public BaseGameObject {
public:

	Guide() {
		CreateTag(this);
	}
	~Guide(){}

	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SetSubmarine(Submarine* submarine);

private:

	std::unique_ptr<Sprite> sprite_;

	Submarine* submarine_ = nullptr;

};