#pragma once


/// ===================================================
/// 描画用コンポーネントのパイプラインなどを初期化と終了処理を行うクラス
/// ===================================================
class RenderComponentInitializer final {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================

	RenderComponentInitializer() = default;
	~RenderComponentInitializer() = default;

	void Initialize();
	void Finalize();
};