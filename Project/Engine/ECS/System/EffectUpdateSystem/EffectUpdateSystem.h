#pragma once

/// engine
#include "../Interface/ECSISystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"

/// /////////////////////////////////////////////////
/// EffectUpdateSystem(エフェクトの更新システム)
/// /////////////////////////////////////////////////
class EffectUpdateSystem : public ECSISystem {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	EffectUpdateSystem() = default;
	~EffectUpdateSystem() = default;

	void RuntimeUpdate(class ECSGroup* _ecs) override;

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	class CameraComponent* mainCamera_ = nullptr;
	Matrix4x4 matBillboard_ = Matrix4x4::kIdentity; ///< ビルボード用の行列

private:
	/// ==================================================
	/// private : methods
	/// ==================================================

	/// @brief エフェクトの要素を更新する
	void UpdateElement(Effect* _effect, Effect::Element* _element);


};

