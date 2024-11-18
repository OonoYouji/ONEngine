#pragma once


/// ===================================================
/// コンポーネントを配列を管理するクラスの基底クラス
/// ===================================================
class IComponentArray {
public:

	virtual ~IComponentArray() {}
	virtual void Update() = 0;

protected:
	static size_t sNextCompId_;
};