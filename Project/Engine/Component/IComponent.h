#pragma once


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
	virtual void PushBackRenderingData(class RenderingPipelineCollection* _collection) = 0;
};

