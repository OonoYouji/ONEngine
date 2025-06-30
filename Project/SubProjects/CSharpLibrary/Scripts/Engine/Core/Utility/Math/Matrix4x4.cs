
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


	static public Matrix4x4 Scale(Vector3 v) {
		return new Matrix4x4(
			v.x, 0, 0, 0,
			0, v.y, 0, 0,
			0, 0, v.z, 0,
			0, 0, 0, 1
		);
	}

	static public Matrix4x4 RotateX(float x) {
		return new Matrix4x4(
			1, 0, 0, 0,
			0, Mathf.Cos(x), Mathf.Sin(x), 0,
			0, -Mathf.Sin(x), Mathf.Cos(x), 0,
			0, 0, 0, 1
		);
	}

	static public Matrix4x4 RotateY(float y) {
		return new Matrix4x4(
			Mathf.Cos(y), 0, -Mathf.Sin(y), 0,
			0, 1, 0, 0,
			Mathf.Sin(y), 0, Mathf.Cos(y), 0,
			0, 0, 0, 1
		);
	}

	static public Matrix4x4 RotateZ(float z) {
		return new Matrix4x4(
			Mathf.Cos(z), Mathf.Sin(z), 0, 0,
			-Mathf.Sin(z), Mathf.Cos(z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}


	static public Matrix4x4 Rotate(Vector3 v) {
		Matrix4x4 rx = RotateX(v.x);
		Matrix4x4 ry = RotateY(v.y);
		Matrix4x4 rz = RotateZ(v.z);

		// Combine the rotations: R = Rz * Ry * Rx
		return rx * ry * rz;
	}

	static public Matrix4x4 Translate(Vector3 v) {
		return new Matrix4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			v.x, v.y, v.z, 1
		);
	}


	static public Vector3 Transform(Matrix4x4 m, Vector3 v) {
		return new Vector3(
			m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30,
			m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31,
			m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32
		);
	}



	public static Matrix4x4 operator *(Matrix4x4 a, Matrix4x4 b) {
		Matrix4x4 result = new Matrix4x4();

		result.m00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30;
		result.m01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31;
		result.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32;
		result.m03 = a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33;

		result.m10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30;
		result.m11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31;
		result.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32;
		result.m13 = a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33;

		result.m20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30;
		result.m21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31;
		result.m22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32;
		result.m23 = a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33;

		result.m30 = a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30;
		result.m31 = a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31;
		result.m32 = a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32;
		result.m33 = a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33;

		return result;
	}
}

