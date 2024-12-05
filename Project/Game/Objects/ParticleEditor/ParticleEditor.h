#pragma once

#include "GameObjectManager/BaseGameObject.h"

class ParticleEditor : public BaseGameObject {

	struct EditData {
		bool  useBillborad;
		float particleLifeTime;
		float emitteTime;

		Vec3  min, max;
		float rateOverTime;
		uint32_t emissionCount;
	};

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	ParticleEditor();
	~ParticleEditor();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	std::list<std::pair<std::string, std::string>> SearchFile(const std::string& directory, const std::string& extension);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	class ParticleSystem* particleSystem_ = nullptr;

	std::string directoryPath_;
	std::string fileName_;

	std::list<std::pair<std::string, std::string>> currentFiles_;

};
