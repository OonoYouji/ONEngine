#pragma once

/// engine
#include "../../EntityComponentSystem/EntityComponentSystem.h"

/// //////////////////////////////////////////////////
/// スカイボックス
/// //////////////////////////////////////////////////
class Skybox : public IEntity {
public:
	/// =============================================
	/// public : methods
	/// =============================================

	Skybox();
	~Skybox();

	void Initialize() override;
	void Update() override;

private:
	/// =============================================
	/// private : objects
	/// =============================================

};

