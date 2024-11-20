#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ModelPreviewObject : public BaseGameObject {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ModelPreviewObject(const std::string& _modelFilePath);
	~ModelPreviewObject();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================


	class AnimationRenderer* animationRenderer_ = nullptr;
	std::string modelFilePath_;

	std::list<std::pair<std::string, std::string>> searchFiles_;

	
	float totalTime_;
};
