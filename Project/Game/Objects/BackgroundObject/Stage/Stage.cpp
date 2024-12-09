#include "Stage.h"

/// std
#include <algorithm>
#include <cassert>

/// engine
#include "VariableManager/VariableManager.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "FrameManager/Time.h"

/// graphics
#include "GraphicManager/ModelManager/ModelManager.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

#include "Math/LerpShortAngle.h"

/// game
#include "Objects/Camera/GameCamera.h"



Stage::Stage() {
	CreateTag(this);
}

Stage::~Stage() {}

void Stage::Initialize() {

	/// ===================================================
	/// setting
	/// ===================================================

	/// component setting
	meshRenderer_ = AddComponent<MeshRenderer>();
	meshRenderer_->SetModel("Stage");


	/// ===================================================
	/// json variable io
	/// ===================================================

}

void Stage::Update() {
}

void Stage::Debug() {
}


