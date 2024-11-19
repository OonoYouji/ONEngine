#pragma once

#include <list>

#include "GameObjectManager/BaseGameObject.h"

class BackgroundObjectManager : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BackgroundObjectManager();
	~BackgroundObjectManager();

	void Initialize() override;
	void Update()     override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::list<BaseGameObject*> bbObjects_;

};
