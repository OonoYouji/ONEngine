#include "Scene_Clear.h"

#include "Objects/Camera/GameCamera.h"

#include "Objects/PlayerHP/PlayerHP.h"

void Scene_Clear::Initialize() {
	(new PlayerHP)->Initialize();
}


void Scene_Clear::Update() {

}
