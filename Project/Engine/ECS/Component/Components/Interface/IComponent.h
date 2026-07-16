#pragma once

/// std
#include <cstdint>
#include <type_traits>


/// ///////////////////////////////////////////////////
/// 全てのコンポーネントの基底クラス
/// ///////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class IComponent
 * @brief すべてのコンポーネントの基底クラス。コンポーネントをアタッチするEntity（GameEntity）の参照や有効・無効状態を保持します。
 */
class IComponent {
	friend class ComponentCollection;
public:
	virtual ~IComponent() = default;

	/**
	 * @brief コンポーネントの内部状態をリセットし、デフォルト状態に戻します。
	 */
	virtual void Reset();

	/**
	 * @brief このコンポーネントを保持するオーナーエンティティ（GameEntity）を設定します。
	 * @param _owner オーナーとなるエンティティポインタ
	 */
	void SetOwner(class GameEntity* _owner);

	/**
	 * @brief このコンポーネントを保持しているオーナーエンティティを取得します。
	 * @return GameEntityポインタ
	 */
	GameEntity* GetOwner() const;


public:
	/// ===========================================
	/// public : objects
	/// ===========================================

	int enable = true; ///< コンポーネントが有効かどうか
	uint32_t id;

protected:
	/// ===========================================
	/// protected : objects
	/// ===========================================

	class GameEntity* owner_ = nullptr;
};

/**
 * @class IRenderComponent
 * @brief 描画処理を伴うコンポーネント（MeshRenderer等）の基底クラス
 */
class IRenderComponent : public IComponent {
public:

};


/// @brief TypeがComponentの型であることを示すコンセプト
template <typename T>
concept IsComponent = std::is_base_of_v<IComponent, T>;

} /// ONEngine
