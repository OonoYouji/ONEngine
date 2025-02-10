#include "Transform.h"


Transform::Transform() {}
Transform::~Transform() {}


void Transform::Update() {
	matWorld = Matrix4x4::MakeAffine(position, rotate, scale);

	if (parent) {
		matWorld = parent->GetMatWorld() * matWorld;
	}
}

void Transform::CreateBuffer(DxDevice* _dxDevice) {
	transformBuffer_ = std::make_unique<ConstantBuffer<BufferData>>();
	transformBuffer_->Create(_dxDevice);
}
