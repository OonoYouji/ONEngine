#pragma once


/// engine
#include "GraphicManager/PostEffect/PipelineRegistry/BasePostEffectPipelineRegistry.h"


/// ===================================================
/// ゲームのポストエフェクトのパイプラインの管理クラス
/// ===================================================
class GamePostEffectPipelineRegistry : public BasePostEffectPipelineRegistry {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GamePostEffectPipelineRegistry();
	~GamePostEffectPipelineRegistry();

};