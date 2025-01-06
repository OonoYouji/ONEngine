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
	stageRenderer_ = AddComponent<MeshRenderer>();
	stageRenderer_->SetModel("Stage");
	stageRenderer_->SetMaterial("StageTile.png");
	stageRenderer_->SetUVScale(Vec2(100.0f, 100.0f));

	wallRenderer = AddComponent<MeshRenderer>();
	wallRenderer->SetModel("Wall");
	wallRenderer->SetMaterial("Wall.png");
	wallRenderer->SetUVScale(Vec2(100.0f, 100.0f));

	/// ===================================================
	/// json variable io
	/// ===================================================

}

void Stage::Update() {
}

void Stage::Debug() {
}


