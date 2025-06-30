struct Mathf {

	static public float Sin(float f) {
		return (float)System.Math.Sin(f);
	}

	static public float Cos(float f) {
		return (float)System.Math.Cos(f);
	}

	static public float Atan2(float y, float x) {
		return (float)System.Math.Atan2(y, x);
	}

	static public float Sqrt(float f) {
		return (float)System.Math.Sqrt(f);
	}

	static public float Rad2Deg {
		get {
			return (float)(180.0 / System.Math.PI);
		}
	}

	static public float Deg2Rad {
		get {
			return (float)(System.Math.PI / 180.0);
		}
	}

}
