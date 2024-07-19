#include <Transform.h>


/// ===================================================
/// 初期化
/// ===================================================
void Transform::Initialize() {
	UpdateMatrix();
}


/// ===================================================
/// 行列の更新
/// ===================================================
void Transform::UpdateMatrix() {
	matTransform = Mat4::MakeAffine(scale, rotate, position);
}
