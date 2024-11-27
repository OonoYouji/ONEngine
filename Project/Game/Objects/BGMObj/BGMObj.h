#pragma once

#include "GameObjectManager/BaseGameObject.h"

class BGMObj : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BGMObj(const std::string& _filePath);
	~BGMObj();

	void Initialize() override;
	void Update()     override;

	void Play();
	void Stop();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class AudioSource* bgm_ = nullptr;
	std::string filePath_;
};
