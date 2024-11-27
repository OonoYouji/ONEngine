#pragma once

#include "GameObjectManager/BaseGameObject.h"

class SEObj : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	SEObj();
	~SEObj();

	void Initialize() override;
	void Update()     override;

	void PlayOneShot(const std::string& _filePath, float _volume);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class AudioSource* se_;
};
