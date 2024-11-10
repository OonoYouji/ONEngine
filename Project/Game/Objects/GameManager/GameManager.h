#pragma once

#include "GameObjectManager/BaseGameObject.h"

class GameManager : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	GameManager();
	~GameManager();

	void Initialize() override;
	void Update()     override;


	/// ===================================================
	/// public : accessor - parameters
	/// ===================================================

	bool GetIsGameEnd() const { return isGameEnd_; }


	/// ===================================================
	/// public : setter - other class pointer
	/// ===================================================

	/// <summary>
	/// レールカメラのポインタをセット
	/// </summary>
	/// <param name="_player"></param>
	void SetRailCamera(class RailCamera* _railCamera);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	/// parameter
	bool isGameEnd_; /// ゲームが終了したかどうか


	/// other class pointer
	class RailCamera* pRailCamera_ = nullptr;

};
