#pragma once

#include <BaseScene.h>
#include"Quaternion.h"
#include"Matrix3x3.h"
#include"Vector3.h"

#include "Player/Player.h"
/// ===================================================
/// ゲームシーン
/// ===================================================
class Scene_Game final : public BaseScene {
public:
	Scene_Game() {}
	~Scene_Game() {}

	Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f));
	Quaternion FromRotationMatrix(const Matrix3x3& m);

	Player* p1;
	void Initialize() override;
	void Update() override;
};