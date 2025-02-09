#pragma once

#include "Engine/Graphics/Pipelines/Collection/RenderingPipelineCollection.h"

/// ===================================================
/// 全てのコンポーネントの基底クラス
/// ===================================================
class IComponent {
public:
	virtual ~IComponent() {}
};

/// ===================================================
/// 描画コンポーネントの基底クラス
/// ===================================================
class IRenderComponent : public IComponent {
public:
	virtual void PushBackRenderingData(RenderingPipelineCollection* _collection) = 0;
};

