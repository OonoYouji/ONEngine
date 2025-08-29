public struct Vector2Int {
	public int x, y;

	public Vector2Int(int _x, int _y) {
		this.x = _x;
		this.y = _y;
	}

	public Vector2Int(Vector2Int other) {
		this.x = other.x;
		this.y = other.y;
	}


	static public readonly Vector2Int zero = new Vector2Int(0, 0);
	static public readonly Vector2Int one = new Vector2Int(1, 1);
	static public readonly Vector2Int up = new Vector2Int(0, 1);
	static public readonly Vector2Int down = new Vector2Int(0, -1);
	static public readonly Vector2Int left = new Vector2Int(1, -1);
	static public readonly Vector2Int right = new Vector2Int(1, 1);
	static public readonly Vector2Int infinity = new Vector2Int(int.MaxValue, int.MaxValue);
	
	public float Length() {
		return Mathf.Sqrt(x * x + y * y);
	}

	public Vector2Int Normalized() {
		float length = this.Length();
		if (length == 0.0f) return zero;
		return new Vector2Int((int)(x / length), (int)(y / length));
	}



	static public Vector2Int Normalized(Vector2Int v) {
		return v.Normalized();
	}

	static public float Length(Vector2Int v) {
		return v.Length();
	}



	static public Vector2Int operator +(Vector2Int a, Vector2Int b) {
		return new Vector2Int(a.x + b.x, a.y + b.y);
	}

	static public Vector2Int operator -(Vector2Int a, Vector2Int b) {
		return new Vector2Int(a.x - b.x, a.y - b.y);
	}

	static public Vector2Int operator *(Vector2Int a, int b) {
		return new Vector2Int(a.x * b, a.y * b);
	}

	static public Vector2Int operator *(int a, Vector2Int b) {
		return new Vector2Int(a * b.x, a * b.y);
	}

	static public Vector2Int operator /(Vector2Int a, int b) {
		if (b == 0) {
			return infinity; // ゼロ除算を避けるために無限大を返す
		}
		return new Vector2Int(a.x / b, a.y / b);
	}

	static public bool operator ==(Vector2Int a, Vector2Int b) {
		return a.x == b.x && a.y == b.y;
	}

	static public bool operator !=(Vector2Int a, Vector2Int b) {
		return !(a == b);
	}

}