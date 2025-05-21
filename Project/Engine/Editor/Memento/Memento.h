#pragma once

/// std
#include <memory>

/// ///////////////////////////////////////////////////
/// Mementoパターン
/// ///////////////////////////////////////////////////
class IMemento {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	IMemento();
	virtual ~IMemento() = default;



private:
	/// =========================================
	/// private : objects
	/// =========================================
	

};



/// ///////////////////////////////////////////////////
/// 状態を持っているクラス、保存と復元を行う
/// ///////////////////////////////////////////////////
class IOriginator {
public:
	/// =========================================
	/// public : methods
	/// =========================================

	IOriginator();
	virtual ~IOriginator() = default;

	virtual std::shared_ptr<IMemento> Save() = 0;
	virtual void Load(const std::shared_ptr<IMemento>& _memento) = 0;
};

