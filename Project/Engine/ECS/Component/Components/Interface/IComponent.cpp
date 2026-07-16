#include "IComponent.h"

using namespace ONEngine;

/**
 * @brief コンポーネントの内部状態をリセットし、デフォルト状態に戻します。
 */
void IComponent::Reset() {}

/**
 * @brief このコンポーネントを保持するオーナーエンティティ（GameEntity）を設定します。
 */
void IComponent::SetOwner(GameEntity* _owner) {
	owner_ = _owner;
}

/**
 * @brief このコンポーネントを保持しているオーナーエンティティを取得します。
 */
GameEntity* IComponent::GetOwner() const {
	return owner_;
}

