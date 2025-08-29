

/// @brief ポストエフェクトの適用
public enum PostEffectFlags : uint {
	None      = 0,      ///< なし
	Lighting  = 1 << 0, ///< ライティング
	Grayscale = 1 << 1, ///< グレースケール
	Bloom     = 1 << 2, ///< ブルーム
	Outline   = 1 << 3, ///< アウトライン
};