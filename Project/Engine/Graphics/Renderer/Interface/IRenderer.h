#pragma once


/* [設計]
 * 
 * すべてのRendererに共通することを抽象化する
 * pipelineは他クラスで管理、Rendererはそれを使う
 * 初めてRendererを使用するときにpipelineを生成してpipelineの管理クラスに登録する
 * 
 */


/// ===================================================
/// 描画のinterfaceクラス
/// ===================================================
class IRenderer {
public:

	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~IRenderer() = default;

	/// @brief 描画のコールを行う
	virtual void DrawCall() = 0;


protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================


};



/// ===================================================
/// 描画のパイプラインのinterfaceクラス
/// ===================================================
class IRenderingPipeline {
public:

	/// ===================================================
	/// public : virtual methods
	/// ===================================================

	virtual ~IRenderingPipeline() = default;

	/// @brief 初期化
	virtual void Initialize() = 0;

	/// @brief 描画前処理
	virtual void PreDraw()    = 0;

	/// @brief 描画後の処理
	virtual void PostDraw()   = 0;


protected:
	
	/// ===================================================
	/// protected : objects
	/// ===================================================


};