#pragma once

#include <CameraManager.h>
#include <Vector3.h>


/// ===================================================
/// ゲームシーン用カメラ
/// ===================================================
class GameCamera final : public BaseCamera {
public:

	GameCamera(const std::string& name);
	~GameCamera();

	void Initialize() override;
	void Update() override;

	/*void Look(const Vector3& pos);*/

	void Debug() override;

private:

	Vec3 LockAt(const Vec3& direction) const;
	Quaternion LockAt(const Vec3& position, const Vec3& target, const Vec3& up) const;

	
};