#pragma once

/// std
#include <vector>

/// engine
#include "../../EntityComponentSystem/EntityComponentSystem.h"

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

	std::vector<VSInput> vsInputs_;
	std::vector<uint32_t> indices_;
public:
	/// =============================================
	/// public : accessors
	/// =============================================

	void SetTexturePath(const std::string& _path);

	const std::string& GetTexturePath() const;
	const std::vector<VSInput>& GetVSInputs() const;
	const std::vector<uint32_t>& GetIndices() const;

};

