#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>

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


namespace COMP_DEBUG {
	void SkyboxDebug(const Skybox* _skybox);
}

void from_json(const nlohmann::json& _j, Skybox& _s);
void to_json(nlohmann::json& _j, const Skybox& _s);
