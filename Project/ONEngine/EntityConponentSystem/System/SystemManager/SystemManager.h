#pragma once

/// std
#include <memory>
#include <vector>

#include "../ISystem/ISystem.h"


class SystemManager final {
public:

	SystemManager() {}
	~SystemManager() {}

	static SystemManager* GetInstance() {
		static SystemManager instance;
		return &instance;
	}


	void Initialize();
	void Update();

private:

	std::vector<std::unique_ptr<ISystem>> systems_;

};