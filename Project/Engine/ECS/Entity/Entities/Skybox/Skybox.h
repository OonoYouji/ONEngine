#pragma once

/// std
#include <vector>

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"

/// //////////////////////////////////////////////////
/// スカイボックス
/// //////////////////////////////////////////////////
class Skybox : public IEntity {
public:

	struct VSInput {
		Vector4 position;
	};

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
	
	std::string texturePath_;

public:
	/// =============================================
	/// public : accessors
	/// =============================================

	void SetTexturePath(const std::string& _path);

	const std::string& GetTexturePath() const;

};

