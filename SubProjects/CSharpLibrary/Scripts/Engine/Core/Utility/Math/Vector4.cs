using System;
using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct Vector4 {
	public float x, y, z, w;

	public Vector4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	/// -----------------------------------------------
	/// public methods
	/// -----------------------------------------------
	public Vector4 Normalized() {
		float length = this.Length();
		if (length == 0.0f) {
			return zero;
		}
		return this / length;
	}

	public float Length() {
		return (float)Math.Sqrt(x * x + y * y + z * z + w * w);
	}

	/// -----------------------------------------------
	/// static public methods
	/// -----------------------------------------------

	static public Vector4 Normalized(Vector4 _v) {
		return _v.Normalized();
	}

	static public float Length(Vector4 _v) {
		return _v.Length();
	}

	static public Vector4 Lerp(Vector4 _v1, Vector4 _v2, float _t) {
		Vector4 result = new Vector4();
		result.x = Mathf.Lerp(_v1.x, _v2.x, _t);
		result.y = Mathf.Lerp(_v1.y, _v2.y, _t);
		result.z = Mathf.Lerp(_v1.z, _v2.z, _t);
		result.w = Mathf.Lerp(_v1.w, _v2.w, _t);
		return result;
	}

	/// -----------------------------------------------
	/// 定数
	/// -----------------------------------------------

	static public readonly Vector4 zero = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	static public readonly Vector4 one = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	static public readonly Vector4 red = new Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	static public readonly Vector4 green = new Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	static public readonly Vector4 blue = new Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	static public readonly Vector4 negativeInfinity = new Vector4(float.NegativeInfinity,  float.NegativeInfinity, float.NegativeInfinity, float.NegativeInfinity);
	static public readonly Vector4 positiveInfinity = new Vector4(float.PositiveInfinity, float.PositiveInfinity, float.PositiveInfinity, float.PositiveInfinity);

	/// ------------------------------------------------
	/// operators
	/// ------------------------------------------------

	static public Vector4 operator +(Vector4 a, Vector4 b) {
		return new Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	static public Vector4 operator -(Vector4 a, Vector4 b) {
		return new Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	static public Vector4 operator *(Vector4 a, float b) {
		return new Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
	}
	static public Vector4 operator /(Vector4 a, float b) {
		/// 例外処理、 inf予防
		if (b == 0.0f) {
			return zero; 
		}

		return new Vector4(a.x / b, a.y / b, a.z / b, a.w / b);
	}

	//static public bool operator ==(Vector4 a, Vector4 b) {
	//	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
	//}

	//static public bool operator !=(Vector4 a, Vector4 b) {
	//	return !(a == b);
	//}


}
