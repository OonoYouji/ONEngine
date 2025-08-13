
using System.Runtime.CompilerServices;

public struct Matrix4x4 {
	public float m00, m01, m02, m03;
	public float m10, m11, m12, m13;
	public float m20, m21, m22, m23;
	public float m30, m31, m32, m33;


	public Matrix4x4(
		float _m00, float _m01, float _m02, float _m03,
		float _m10, float _m11, float _m12, float _m13,
		float _m20, float _m21, float _m22, float _m23,
		float _m30, float _m31, float _m32, float _m33) {
		
		this.m00 = _m00;
		this.m01 = _m01;
		this.m02 = _m02;
		this.m03 = _m03;
		this.m10 = _m10;
		this.m11 = _m11;
		this.m12 = _m12;
		this.m13 = _m13;
		this.m20 = _m20;
		this.m21 = _m21;
		this.m22 = _m22;
		this.m23 = _m23;
		this.m30 = _m30;
		this.m31 = _m31;
		this.m32 = _m32;
		this.m33 = _m33;
	}


	/// =================================
	/// methods
	/// =================================


	static public Matrix4x4 Scale(Vector3 _v) {
		return new Matrix4x4(
			_v.x, 0, 0, 0,
			0, _v.y, 0, 0,
			0, 0, _v.z, 0,
			0, 0, 0, 1
		);
	}

	static public Matrix4x4 RotateX(float _x) {
		return new Matrix4x4(
			1, 0, 0, 0,
			0, Mathf.Cos(_x), Mathf.Sin(_x), 0,
			0, -Mathf.Sin(_x), Mathf.Cos(_x), 0,
			0, 0, 0, 1
		);
	}

	static public Matrix4x4 RotateY(float _y) {
		return new Matrix4x4(
			Mathf.Cos(_y), 0, -Mathf.Sin(_y), 0,
			0, 1, 0, 0,
			Mathf.Sin(_y), 0, Mathf.Cos(_y), 0,
			0, 0, 0, 1
		);
	}

	static public Matrix4x4 RotateZ(float _z) {
		return new Matrix4x4(
			Mathf.Cos(_z), Mathf.Sin(_z), 0, 0,
			-Mathf.Sin(_z), Mathf.Cos(_z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}


	static public Matrix4x4 Rotate(Vector3 _v) {
		Matrix4x4 rx = RotateX(_v.x);
		Matrix4x4 ry = RotateY(_v.y);
		Matrix4x4 rz = RotateZ(_v.z);

		// Combine the rotations: R = Rz * Ry * Rx
		return rx * ry * rz;
	}

	static public Matrix4x4 Rotate(Quaternion _q) {
		if (Quaternion.Length(_q) == 0.0f) {
			return new Matrix4x4();
		}
		Matrix4x4 result = new Matrix4x4();

		float ww = _q.w * _q.w;
		float xx = _q.x * _q.x;
		float yy = _q.y * _q.y;
		float zz = _q.z * _q.z;
		float wx = _q.w * _q.x;
		float wy = _q.w * _q.y;
		float wz = _q.w * _q.z;
		float xy = _q.x * _q.y;
		float xz = _q.x * _q.z;
		float yz = _q.y * _q.z;

		result.m00 = ww + xx - yy - zz;
		result.m01 = 2 * (xy + wz);
		result.m02 = 2 * (xz - wy);

		result.m10 = 2 * (xy - wz);
		result.m11 = ww - xx + yy - zz;
		result.m12 = 2 * (yz + wx);

		result.m20 = 2 * (xz + wy);
		result.m21 = 2 * (yz - wx);
		result.m22 = ww - xx - yy + zz;

		result.m33 = 1.0f;

		return result;
	}

	static public Matrix4x4 Translate(Vector3 _v) {
		return new Matrix4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			_v.x, _v.y, _v.z, 1
		);
	}


	static public Vector3 Transform(Vector3 _v, Matrix4x4 _m) {
		return new Vector3(
			_m.m00 * _v.x + _m.m10 * _v.y + _m.m20 * _v.z + _m.m30,
			_m.m01 * _v.x + _m.m11 * _v.y + _m.m21 * _v.z + _m.m31,
			_m.m02 * _v.x + _m.m12 * _v.y + _m.m22 * _v.z + _m.m32
		);
	}



	public static Matrix4x4 operator *(Matrix4x4 _a, Matrix4x4 _b) {
		Matrix4x4 result = new Matrix4x4();

		result.m00 = _a.m00 * _b.m00 + _a.m01 * _b.m10 + _a.m02 * _b.m20 + _a.m03 * _b.m30;
		result.m01 = _a.m00 * _b.m01 + _a.m01 * _b.m11 + _a.m02 * _b.m21 + _a.m03 * _b.m31;
		result.m02 = _a.m00 * _b.m02 + _a.m01 * _b.m12 + _a.m02 * _b.m22 + _a.m03 * _b.m32;
		result.m03 = _a.m00 * _b.m03 + _a.m01 * _b.m13 + _a.m02 * _b.m23 + _a.m03 * _b.m33;

		result.m10 = _a.m10 * _b.m00 + _a.m11 * _b.m10 + _a.m12 * _b.m20 + _a.m13 * _b.m30;
		result.m11 = _a.m10 * _b.m01 + _a.m11 * _b.m11 + _a.m12 * _b.m21 + _a.m13 * _b.m31;
		result.m12 = _a.m10 * _b.m02 + _a.m11 * _b.m12 + _a.m12 * _b.m22 + _a.m13 * _b.m32;
		result.m13 = _a.m10 * _b.m03 + _a.m11 * _b.m13 + _a.m12 * _b.m23 + _a.m13 * _b.m33;

		result.m20 = _a.m20 * _b.m00 + _a.m21 * _b.m10 + _a.m22 * _b.m20 + _a.m23 * _b.m30;
		result.m21 = _a.m20 * _b.m01 + _a.m21 * _b.m11 + _a.m22 * _b.m21 + _a.m23 * _b.m31;
		result.m22 = _a.m20 * _b.m02 + _a.m21 * _b.m12 + _a.m22 * _b.m22 + _a.m23 * _b.m32;
		result.m23 = _a.m20 * _b.m03 + _a.m21 * _b.m13 + _a.m22 * _b.m23 + _a.m23 * _b.m33;

		result.m30 = _a.m30 * _b.m00 + _a.m31 * _b.m10 + _a.m32 * _b.m20 + _a.m33 * _b.m30;
		result.m31 = _a.m30 * _b.m01 + _a.m31 * _b.m11 + _a.m32 * _b.m21 + _a.m33 * _b.m31;
		result.m32 = _a.m30 * _b.m02 + _a.m31 * _b.m12 + _a.m32 * _b.m22 + _a.m33 * _b.m32;
		result.m33 = _a.m30 * _b.m03 + _a.m31 * _b.m13 + _a.m32 * _b.m23 + _a.m33 * _b.m33;

		return result;
	}
}

