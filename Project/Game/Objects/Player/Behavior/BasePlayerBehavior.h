#pragma once

#include <string>

class BasePlayerBehavior {
public:

	BasePlayerBehavior(class Player* _player)
		: player_(_player) {
	}

	virtual ~BasePlayerBehavior() = default;

	virtual void Initialize() = 0; /// 状態開始時の処理
	virtual void Update()     = 0; /// 状態中の処理
	virtual void Finalize()   = 0; /// 状態終了時の処理


	/// <summary>
	/// 次のbehaviorの名前
	/// </summary>
	/// <returns></returns>
	virtual std::string GetNextBehavior() = 0;

private:
	class Player* player_ = nullptr;
};