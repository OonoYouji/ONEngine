using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct Vector2 {
	public float x, y;

	public Vector2(float _x, float _y) {
		this.x = _x;
		this.y = _y;
	}

	public Vector2(Vector2 other) {
		this.x = other.x;
		this.y = other.y;
	}


	public float Length() {
		return (float)System.Math.Sqrt(x * x + y * y);
	}

	public Vector2 Normalized() {
		float length = this.Length();
		if (length == 0.0f) return zero;
		return new Vector2(x / length, y / length);
	}



	static public Vector2 Normalized(Vector2 v) {
		return v.Normalized();
	}
	
	static public float Length(Vector2 v) {
		return v.Length();
	}

	static public float Dot(Vector2 a, Vector2 b) {
		return a.x * b.x + a.y * b.y;
	}



	/// ------------------------------------------------
	/// 定数
	/// ------------------------------------------------

	static public Vector2 zero {
		get { return new Vector2(0.0f, 0.0f); }
	}

	static public Vector2 one {
		get { return new Vector2(1.0f, 1.0f); }
	}

	static public Vector2 up {
		get { return new Vector2(0.0f, 1.0f); }
	}

	static public Vector2 down {
		get { return new Vector2(0.0f, -1.0f); }
	}

	static public Vector2 left {
		get { return new Vector2(-1.0f, 0.0f); }
	}

	static public Vector2 right {
		get { return new Vector2(1.0f, 0.0f); }
	}

	static public Vector2 positiveInfinity {
		get { return new Vector2(float.PositiveInfinity, float.PositiveInfinity); }
	}

	static public Vector2 negativeInfinity {
		get { return new Vector2(float.NegativeInfinity, float.NegativeInfinity); }
	}


	/// ------------------------------------------------
	/// operators
	/// ------------------------------------------------

	static public Vector2 operator +(Vector2 a, Vector2 b) {
		return new Vector2(a.x + b.x, a.y + b.y);
	}

	static public Vector2 operator -(Vector2 a, Vector2 b) {
		return new Vector2(a.x - b.x, a.y - b.y);
	}

	static public Vector2 operator *(Vector2 a, float b) {
		return new Vector2(a.x * b, a.y * b);
	}

	static public Vector2 operator /(Vector2 a, float b) {
		if (b == 0.0f) return zero; // Avoid division by zero
		return new Vector2(a.x / b, a.y / b);
	}

	static public Vector2 operator *(float a, Vector2 b) {
		return new Vector2(a * b.x, a * b.y);
	}

	static public Vector2 operator /(float a, Vector2 b) {
		if (b.x == 0.0f || b.y == 0.0f) return zero; // Avoid division by zero
		return new Vector2(a / b.x, a / b.y);
	}






}
