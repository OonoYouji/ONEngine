#pragma once


/// ===================================================
/// 全てのコンポーネントの基底クラス
/// ===================================================
class IComponent {
public:
	virtual ~IComponent() {}

	/// @brief このcomponentのownerを設定
	/// @param _owner オーナーとなるentity
	void SetOwner(class IEntity* _owner) { owner_ = _owner; }

protected:
	class IEntity* owner_ = nullptr;
};

/// ===================================================
/// 描画コンポーネントの基底クラス
/// ===================================================
class IRenderComponent : public IComponent {
public:
	virtual void PushBackRenderingData(class RenderingPipelineCollection* _collection) = 0;
};

