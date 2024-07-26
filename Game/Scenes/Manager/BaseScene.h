#pragma once


/// ===================================================
/// 各シーンの基底クラス
/// ===================================================
class BaseScene {
public:

	virtual ~BaseScene() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

};