#pragma once

/// std
#include <cstdint>


/// ===================================================
/// 全てのコンポーネントの基底クラス
/// ===================================================
class IComponent {
	friend class ComponentCollection;
public:
	virtual ~IComponent() = default;

	/// @brief このcomponentのownerを設定
	/// @param _owner オーナーとなるentity
	void SetOwner(class IEntity* _owner);

	/// @brief ownerの取得
	/// @return ownerのentity
	IEntity* GetOwner() const;

public:
	/// ===========================================
	/// public : objects
	/// ===========================================

	bool enable = true; ///< コンポーネントが有効かどうか
	uint32_t id;

protected:
	/// ===========================================
	/// protected : objects
	/// ===========================================

	class IEntity* owner_ = nullptr;
};

/// ===================================================
/// 描画コンポーネントの基底クラス
/// ===================================================
class IRenderComponent : public IComponent {
public:

};

