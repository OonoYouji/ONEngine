#pragma once

/// ===================================================
/// ECS System Interface class
/// ===================================================
class ISystem {
public:

	virtual ~ISystem() {}

	/// <summary>
	/// 更新処理関数
	/// </summary>
	/// <param name="_entity">     : 更新処理を行うEntity     </param>
	/// <param name="_ecsManager"> : ECSManagerへのポインター </param>
	virtual void Execute(size_t _entity, class ECSManager* _ecsManager) = 0;

};