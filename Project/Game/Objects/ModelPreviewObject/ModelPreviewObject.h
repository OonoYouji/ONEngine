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


	class MeshRenderer* meshRenderer_ = nullptr;
	std::string modelFilePath_;

};
