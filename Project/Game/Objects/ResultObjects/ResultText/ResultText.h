#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ResultText : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ResultText(const std::string& _filePath);
	~ResultText();

	void Initialize() override;
	void Update()     override;

	void AddVariables();
	void ApplyVariables();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class SpriteRenderer* renderer_ = nullptr;
	std::string filePath_;

};
