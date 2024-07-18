#include <Matrix4x4.h>

#include <Vector4.h>


namespace {

	inline XMMATRIX ComputeInverseMatrix(const XMMATRIX& matrix) {
		XMVECTOR determinant;
		XMMATRIX inverseMatrix = XMMatrixInverse(&determinant, matrix);
		return inverseMatrix;
	}

}


/// ===================================================
/// static objects initialize
/// ===================================================

const Matrix4x4 Matrix4x4::kIdentity = Matrix4x4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { 0,0,0,1 });



Matrix4x4::Matrix4x4() {
	(*this) = kIdentity;
}

Matrix4x4::Matrix4x4(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4) {
	this->m[0][0] = row1.x;
	this->m[0][1] = row1.y;
	this->m[0][2] = row1.z;
	this->m[0][3] = row1.w;

	this->m[1][0] = row2.x;
	this->m[1][1] = row2.y;
	this->m[1][2] = row2.z;
	this->m[1][3] = row2.w;

	this->m[2][0] = row3.x;
	this->m[2][1] = row3.y;
	this->m[2][2] = row3.z;
	this->m[2][3] = row3.w;

	this->m[3][0] = row4.x;
	this->m[3][1] = row4.y;
	this->m[3][2] = row4.z;
	this->m[3][3] = row4.w;

}

Matrix4x4::Matrix4x4(const XMMATRIX& xm) {
	(*this) = ConvertToMatrix4x4(xm);
}

Matrix4x4 Matrix4x4::Inverse() {
	return Matrix4x4(ComputeInverseMatrix(ConvertToXMMATRIX((*this))));
}

