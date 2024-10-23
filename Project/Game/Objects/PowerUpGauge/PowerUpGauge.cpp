#include "PowerUpGauge.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"


PowerUpGauge::PowerUpGauge() {
	CreateTag(this);
}

PowerUpGauge::~PowerUpGauge() {}

void PowerUpGauge::Initialize() {
	drawLayerId = 1;

	MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("gage");



}

void PowerUpGauge::Update() {

}
