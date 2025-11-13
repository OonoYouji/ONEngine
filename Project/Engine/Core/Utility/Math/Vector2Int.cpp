#include "Vector2Int.h"

Vector2Int::Vector2Int() = default;
Vector2Int::Vector2Int(int _x, int _y) : x(_x), y(_y) {}
Vector2Int::Vector2Int(const Vector2Int& _v) : x(_v.x), y(_v.y) {}
Vector2Int::Vector2Int(Vector2Int&&) = default;


/// ===================================================
/// static objects
/// ===================================================
Vector2Int Vector2Int::kOne = Vector2Int(1, 1);   ///< (1, 1)
Vector2Int Vector2Int::kZero = Vector2Int(0, 0);  ///< (0, 0)
Vector2Int Vector2Int::kUp = Vector2Int(0, 1);    ///< (0, 1)
Vector2Int Vector2Int::kDown = Vector2Int(0, -1); ///< (0, -1)
Vector2Int Vector2Int::kLeft = Vector2Int(-1, 0); ///< (-1, 0)
Vector2Int Vector2Int::kRight = Vector2Int(1, 0); ///< (1, 0)

Vector2Int& Vector2Int::operator=(const Vector2Int& _other) {
	this->x = _other.x;
	this->y = _other.y;
	return *this;
}
