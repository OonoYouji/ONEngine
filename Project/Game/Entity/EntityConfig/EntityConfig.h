#pragma once

/// ///////////////////////////////////////////////////
/// 全体で共通する値
/// ///////////////////////////////////////////////////
struct GameConfig {
	static const float kBlockSize; ///< ブロックのサイズ
};





/// ///////////////////////////////////////////////////
/// タイプ
/// ///////////////////////////////////////////////////
enum class Type {
	black, ///< 黒
	white, ///< 白
};


/// ///////////////////////////////////////////////////
/// ステージ上のアドレス
/// ///////////////////////////////////////////////////
struct Address {
	int row; ///< 行
	int col; ///< 列

	inline Address& operator= (const Address& _address) {
		row = _address.row;
		col = _address.col;
		return *this;
	}
};


/// @brief アドレスの比較 全一致でtrue
inline bool operator== (const Address& _lhs, const Address& _rhs) {
	return _lhs.row == _rhs.row && _lhs.col == _rhs.col;
}

inline Address operator+ (const Address& _lhs, const Address& _rhs) {
	return { _lhs.row + _rhs.row, _lhs.col + _rhs.col };
}