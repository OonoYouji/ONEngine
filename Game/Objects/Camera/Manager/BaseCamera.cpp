#include <BaseCamera.h>

#include <CameraManager.h>


/// ===================================================
/// コンストラクタ
/// ===================================================
BaseCamera::BaseCamera() {

}



/// ===================================================
/// 初期化
/// ===================================================
void BaseCamera::BaseInitialize() {

	matWorld_ = Mat4::MakeAffine(Vec3::kOne, rotate_, position_);
	matView_ = matWorld_.Inverse();
	

}
