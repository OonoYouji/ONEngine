#pragma once

#include "Engine/Entity/Interface/IEntity.h"

/// ==================================================================================
/// Block クラス
/// ==================================================================================
class Block : public IEntity {
public:

	Block();
	~Block();

	void Initialize() override;
	void Update()     override;

private:

	

};

