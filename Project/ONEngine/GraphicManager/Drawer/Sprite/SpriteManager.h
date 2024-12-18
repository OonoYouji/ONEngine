#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <memory>
#include <list>
#include <cstdint>

#include "GraphicManager/PipelineState/PipelineState.h"
#include "GraphicManager/Drawer/Sprite/Sprite.h"

#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"


/// ===================================================
/// 2Dテクスチャの管理クラス
/// ===================================================
class SpriteManager final {
	SpriteManager() = default;
	~SpriteManager() = default;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================


	/// <summary>
	/// インスタンス確保
	/// </summary>
	static SpriteManager* GetInstance();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();


	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();


	/// <summary>
	/// アクティブなスプライトの追加
	/// </summary>
	/// <param name="sprite"></param>
	void AddActiveSprite(Sprite* sprite);


private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::unique_ptr<PipelineState> pipelineState_;
	PipelineState::Shader shader_;

	struct ActiveSprite {
		Sprite* pSprite_ = nullptr;
	};
	std::list<ActiveSprite> activeSprites_;

private:
	SpriteManager(const SpriteManager&) = delete;
	SpriteManager(SpriteManager&&) = delete;
	SpriteManager& operator=(const SpriteManager&) = delete;
	SpriteManager& operator=(SpriteManager&&) = delete;
};