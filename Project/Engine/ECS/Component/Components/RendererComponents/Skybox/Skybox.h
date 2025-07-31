#pragma once

/// std
#include <string>

/// engine
#include "../../Interface/IComponent.h"

/// ///////////////////////////////////////////////////
/// Skyboxのコンポーネントクラス
/// ///////////////////////////////////////////////////
class Skybox : public IRenderComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Skybox();
	~Skybox() override;

	void SetDDSTexturePath(const std::string& _texturePath);
	const std::string& GetDDSTexturePath() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string texturePath_;

};