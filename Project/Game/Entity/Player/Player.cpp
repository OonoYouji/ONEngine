#include "Player.h"

/// engine
#include "Engine/Component/RendererComponents/Mesh/MeshRenderer.h"

Player::Player() {}
Player::~Player() {}

void Player::Initialize() {

	MeshRenderer* renderer = AddComponent<MeshRenderer>();
	renderer->SetMeshPath("Assets/Models/entity/player.obj");
	renderer->SetTexturePath("Assets/Textures/uvChecker.png");

}

void Player::Update() {

	MeshRenderer* renderer = GetComponent<MeshRenderer>();
	renderer->SetColor(type_ == static_cast<int>(BlockType::black) ? Vector4(0, 0, 0, 1) : Vector4::kWhite);

}
