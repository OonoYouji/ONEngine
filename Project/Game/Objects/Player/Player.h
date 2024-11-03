#pragma once
#include "GameObjectManager/BaseGameObject.h"

class Player :
	public BaseGameObject{
public:
	void Initialize()override;
	void Update()override;

private:
	class MeshRenderer* meshRenderer_ = nullptr;
};

