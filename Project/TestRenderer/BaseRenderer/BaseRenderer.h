#pragma once

/* ===================================================
	BaseRenderer
	Rendererの基底クラス

	## やりたいこと
	- Model, Line, Sprite, AnimationなどのRendererを抽象化して描画したい
	- DirectX12のcommand listをsingletonで持っているので、それをやめる


	## どうやって？？
	- command listのクラスのポインタを引数で指定する
		- graphics engine部分でrendererを登録する
			- model等がどのrendererを使うかを指定する




=================================================== */



/// std
#include <memory>

/// directX12
#include <wrl/client.h>
#include <d3d12.h>

#include "GraphicManager/PipelineState/PipelineState.h"


/// using namespace
using namespace Microsoft::WRL;


/// ===================================================
/// 描画の基底クラス
/// ===================================================
class BaseRenderer {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BaseRenderer(ID3D12GraphicsCommandList* _commandList);
	~BaseRenderer() {}

	virtual void Initialize() = 0;
	
	virtual void PreDraw()  = 0;
	virtual void PostDraw() = 0;

protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	ID3D12GraphicsCommandList*     commandList_   = nullptr;

	/// graphics pipeline state
	std::unique_ptr<PipelineState> pipelineState_ = nullptr;

};