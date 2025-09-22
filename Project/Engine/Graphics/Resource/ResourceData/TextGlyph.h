#pragma once

/// std
#include <cstdint>
#include <vector>

/// engine
#include "Engine/Core/Utility/Math/Vector2Int.h"


/// ////////////////////////////////////////////////////////////////
/// 1文字あたりのデータ
/// ////////////////////////////////////////////////////////////////
struct Glyph {
	uint32_t codepoint;   // Unicodeコードポイント
	int width;            // ビットマップ幅
	int height;           // ビットマップ高さ
	int bearingX;         // 左端からのオフセット
	int bearingY;         // ベースラインから上方向へのオフセット
	int advance;          // 次の文字までの進み幅(px単位)

	Vector2Int min;       // グリフの左下の位置（オフセット込み）
	Vector2Int max;       // グリフの右上の位置（オフセット込み）
	std::vector<unsigned char> bitmap; // グリフのアルファチャンネル
};

/// ////////////////////////////////////////////////////////////////
/// テキストのデータ
/// ////////////////////////////////////////////////////////////////
class Text {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Text();
	~Text();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<Glyph> glyphs_;

	/// ----- サイズ ----- ///
	float pixelHeight_;
	
	/// ----- 基本情報 ----- ///
	int ascent_;
	int descent_;
	int lineGap_;
	float baseline_;
	float lineHeight_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================


};