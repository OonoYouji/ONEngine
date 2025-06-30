#include "BoxCollider.h"

BoxCollider::BoxCollider() {
	// デフォルトの値をセット
	size_ = Vector3::kOne; // サイズを1x1x1に初期化
}

void BoxCollider::SetSize(const Vector3& _size) {
	size_ = _size;
}

const Vector3& BoxCollider::GetSize() const {
	return size_;
}
