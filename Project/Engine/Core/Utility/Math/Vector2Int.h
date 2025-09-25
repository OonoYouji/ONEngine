#pragma once

/// ////////////////////////////////////////////////
/// 2次元整数ベクトル
/// ////////////////////////////////////////////////
class Vector2Int final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Vector2Int();
	Vector2Int(int _x, int _y);
	Vector2Int(const Vector2Int& _v);
	Vector2Int(Vector2Int&& _v);

	/// ===================================================
	/// public : objects
	/// ===================================================

	int x, y;


	/// ===================================================
	/// public : static objects
	/// ===================================================

	static Vector2Int kOne;  ///< (1, 1)
	static Vector2Int kZero; ///< (0, 0)
	static Vector2Int kUp;   ///< (0, 1)
	static Vector2Int kDown; ///< (0, -1)
	static Vector2Int kLeft; ///< (-1, 0)
	static Vector2Int kRight;///< (1, 0)


	/// ===================================================
	/// public : static methods
	/// ===================================================



};