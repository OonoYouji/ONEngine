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
namespace ONEngine {

class Skybox : public IRenderComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Skybox();
	~Skybox() override;


	/// @brief DDSテクスチャのパスを設定する
	/// @param _texturePath テクスチャのパス
	void SetDDSTexturePath(const std::string& _texturePath);

	/// @brief DDSテクスチャのパスを返す
	const std::string& GetDDSTexturePath() const;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string texturePath_;

};


namespace ComponentDebug {
	void SkyboxDebug(const Skybox* _skybox);
}

/// Json変換
void from_json(const nlohmann::json& _j, Skybox& _s);
void to_json(nlohmann::json& _j, const Skybox& _s);

} /// ONEngine