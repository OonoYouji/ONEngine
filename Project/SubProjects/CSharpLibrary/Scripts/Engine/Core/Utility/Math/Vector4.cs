using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class Vector4 {
	public float x, y, z, w;

	public Vector4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	public Vector4() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
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


	/// -----------------------------------------------
	/// 定数
	/// -----------------------------------------------

	static public Vector4 zero {
		get { return new Vector4(0.0f, 0.0f, 0.0f, 0.0f); }
	}
	static public Vector4 one {
		get { return new Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
	}

	static public Vector4 negativeInfinity {
		get { return new Vector4(float.NegativeInfinity, float.NegativeInfinity, float.NegativeInfinity, float.NegativeInfinity); }
	}

	static public Vector4 positiveInfinity {
		get { return new Vector4(float.PositiveInfinity, float.PositiveInfinity, float.PositiveInfinity, float.PositiveInfinity); }
	}


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
