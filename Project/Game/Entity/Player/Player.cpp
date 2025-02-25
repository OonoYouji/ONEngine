#include "Player.h"

/// engine
#include "Engine/Component/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/Core/Utility/Input/Input.h"


Player::Player() {}
Player::~Player() {}

void Player::Initialize() {

	SpriteRenderer* renderer = AddComponent<SpriteRenderer>();
	renderer->SetTexturePath("Assets/Textures/uvChecker.png");

}

void Player::Update() {
	
	{	/// 移動処理



	}

}
