using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
public struct Vector3 {
	public float x, y, z;

	/// =================================
	/// constructor
	/// =================================

	public Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
		this.x = x;
		this.y = y;
		this.z = z;
	}

	public Vector3(Vector3 other) {
		this.x = other.x;
		this.y = other.y;
		this.z = other.z;
	}

	/// =================================
	/// methods
	/// =================================

	public float length() {
		return (float)System.Math.Sqrt(x * x + y * y + z * z);
	}

	public Vector3 normalized() {
		float length = this.length();
		if (length == 0.0f) return zero;
		return new Vector3(x / length, y / length, z / length);
	}


	/// =================================
	/// 定数
	/// =================================

	public static Vector3 zero {
		get { return new Vector3(0.0f, 0.0f, 0.0f); }
	}

	public static Vector3 one {
		get { return new Vector3(1.0f, 1.0f, 1.0f); }
	}

	public static Vector3 up {
		get { return new Vector3(0.0f, 1.0f, 0.0f); }
	}

	public static Vector3 down {
		get { return new Vector3(0.0f, -1.0f, 0.0f); }
	}

	public static Vector3 left {
		get { return new Vector3(-1.0f, 0.0f, 0.0f); }
	}

	public static Vector3 right {
		get { return new Vector3(1.0f, 0.0f, 0.0f); }
	}

	public static Vector3 forward {
		get { return new Vector3(0.0f, 0.0f, 1.0f); }
	}

	public static Vector3 back {
		get { return new Vector3(0.0f, 0.0f, -1.0f); }
	}


	/// =================================
	/// operators
	/// =================================

	public static Vector3 operator +(Vector3 a, Vector3 b) {
		return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	public static Vector3 operator -(Vector3 a, Vector3 b) {
		return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	public static Vector3 operator *(Vector3 a, float scalar) {
		return new Vector3(a.x * scalar, a.y * scalar, a.z * scalar);
	}

	public static Vector3 operator /(Vector3 a, float scalar) {
		return new Vector3(a.x / scalar, a.y / scalar, a.z / scalar);
	}

}

