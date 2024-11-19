#include "BackgroundObjectManager.h"

#include "../Building/Building.h"
#include "../Stage/Stage.h"
#include "../SkyDome/SkyDome.h"

BackgroundObjectManager::BackgroundObjectManager() {
	CreateTag(this);
}

BackgroundObjectManager::~BackgroundObjectManager() {}

void BackgroundObjectManager::Initialize() {

	bbObjects_ = {
		new SkyDome(),
		new Stage(),
		new Building()
	};


	for(auto& obj : bbObjects_) {
		obj->Initialize();
	}

}

void BackgroundObjectManager::Update() {

}

