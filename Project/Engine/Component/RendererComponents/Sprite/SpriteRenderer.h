#pragma once

/// std
#include <string>

/// engine
#include "../../Interface/IComponent.h"

/// ===================================================
/// sprite描画クラス
/// ===================================================
class SpriteRenderer final : public IComponent {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderer();
	~SpriteRenderer();


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string texturePath_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief 描画するtextureの file pathを設定
	/// @param _path .slnからの相対パス
	void SetTexturePath(const std::string& _path) { texturePath_ = _path; }


	/// @brief 描画するtextureの file pathを取得
	/// @return .slからの相対パス
	const std::string& GetTexturePath() const { return texturePath_; }

};

