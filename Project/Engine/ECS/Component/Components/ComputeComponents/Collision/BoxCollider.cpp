#include "BoxCollider.h"

/// externals
#include <imgui.h>

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

void COMP_DEBUG::BoxColliderDebug(BoxCollider* _bc) {
	if (!_bc) {
		return;
	}
	/// サイズの取得
	Vector3 size = _bc->GetSize();
	/// サイズの編集
	if (ImGui::DragFloat3("Box Collider Size", &size.x, 0.1f, 0.0f, FLT_MAX)) {
		_bc->SetSize(size);
	}
}
