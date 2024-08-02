#include <Transform.h>

#include <DxResourceCreator.h>

/// ===================================================
/// 初期化
/// ===================================================
void Transform::Initialize() {
	transformBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapingData_));
	UpdateMatrix();
}


/// ===================================================
/// 行列の更新
/// ===================================================
void Transform::UpdateMatrix() {
	matTransform = Mat4::MakeAffine(scale, rotate, position);
	*mapingData_ = matTransform;
}

void Transform::BindTransform(ID3D12GraphicsCommandList* commandList, UINT rootParamIndex) {
	commandList->SetGraphicsRootConstantBufferView(rootParamIndex, transformBuffer_->GetGPUVirtualAddress());
}
