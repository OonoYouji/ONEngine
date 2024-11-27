#pragma once

/// std
#include <string>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"

class BasePlayerBehavior {
public:

	BasePlayerBehavior(const std::string& _name, class Player* _player)
		: player_(_player), kName_(_name) {
	}

	virtual ~BasePlayerBehavior() = default;

	virtual void Start()  = 0; /// 状態開始時の処理
	virtual void Update() = 0; /// 状態中の処理
	virtual void Exit()   = 0; /// 状態終了時の処理

	/// <summary>
	/// 次のbehaviorの名前
	/// </summary>
	/// <returns></returns>
	virtual std::string GetNextBehavior() = 0;

	virtual bool CanExit() = 0;

public:

	/// jsonのio用
	virtual void AddVariables() {}
	virtual void ApplyVariables() {}

protected:
	class Player* player_ = nullptr;
public:
	const std::string kName_;
	static const std::string sDirectoryPath_;
};
