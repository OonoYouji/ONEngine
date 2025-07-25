using System.Runtime.CompilerServices;

struct Mathf {

	/// ------------------------------------------
	/// 基礎計算
	/// ------------------------------------------

	static public float Sin(float f) {
		return (float)System.Math.Sin(f);
	}

	static public float Cos(float f) {
		return (float)System.Math.Cos(f);
	}

	static public float Atan2(float y, float x) {
		return (float)System.Math.Atan2(y, x);
	}

	static public float Asin(float f) {
		return (float)System.Math.Asin(f);
	}

	static public float Acos(float f) {
		return (float)System.Math.Acos(f);
	}

	static public float Tan(float f) {
		return (float)System.Math.Tan(f);
	}

	static public float Abs(float f) {
		return (float)System.Math.Abs(f);
	}

	static public float Sqrt(float f) {
		return (float)System.Math.Sqrt(f);
	}

	/// --------------------------------------------
	/// 便利な計算
	/// --------------------------------------------

	static public float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}

	static public float Clamp(float value, float min, float max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	static public float Clamp01(float value) {
		// 0.0f から 1.0f の範囲に制限
		if (value < 0.0f) return 0.0f;
		if (value > 1.0f) return 1.0f;
		return value;
	}

	static public float Sign(float value) {
		// value の符号を返す
		if (value < 0.0f) {
			return -1.0f;
		} else if (value > 0.0f) {
			return 1.0f;
		} else {
			return 0.0f; // value が 0 の場合は 0 を返す
		}
	}

	static public float CopySign(float value, float sign) {
		return Mathf.Abs(value) * Mathf.Sign(sign);
	}



	/// --------------------------------------------
	/// 回転角の変換 定数
	/// --------------------------------------------

	static public float PI {
		get {
			return (float)System.Math.PI;
		}
	}

	static public float Rad2Deg {
		get {
			return 180.0f / Mathf.PI;
		}
	}

	static public float Deg2Rad {
		get {
			return Mathf.PI / 180.0f;
		}
	}


	/// --------------------------------------------
	/// ファイル探索
	/// --------------------------------------------

	[MethodImpl(MethodImplOptions.InternalCall)]
	static public extern string LoadFile(string _path);


}
