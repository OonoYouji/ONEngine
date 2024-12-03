#pragma once

/// std
#include <unordered_map>
#include <string>
#include <functional>

/// engine
#include "GameObjectManager/BaseGameObject.h"

class BackgroundObjectManager : public BaseGameObject {

	struct IOData {
		Vec3 position;
		Vec3 rotate;
		Vec3 scale;
		std::string className;
	};

	using FactoryFunction = std::function<BaseGameObject*()>;

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	BackgroundObjectManager();
	~BackgroundObjectManager();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;


	void SaveFile(const std::string& _filePath);
	void LoadFile(const std::string& _filePath);

	void CreateObject(const IOData& _ioData);

	void Register(const std::string& _className, FactoryFunction _factoryFunction);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	const std::string directoryFile_ = "./Resources/Parameters/BackgroundObjectManager/";

	std::list<BaseGameObject*> bbObjects_;


	/// jsonのio用
	std::vector<IOData>                              objectIOData_;
	int                                              selectIOData_;

	IOData                                           srcObject_;
	std::vector<std::string>                         objectClassNames_;
	int                                              currentItemIndex_;

	std::unordered_map<std::string, FactoryFunction> factoryMap_;

};
