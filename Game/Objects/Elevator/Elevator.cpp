#include "Elevator.h"

#include <ModelManager.h>
#include <WorldTime.h>

#include "Wire/Wire.h"



void Elevator::Initialize() {
	model_ = ModelManager::Load("Elevator");
}


void Elevator::Update() {
	if(wire_) {

		transform_.position.y += -0.25f * WorldTime::DeltaTime();

	}
}


void Elevator::Draw() {
	model_->Draw(&transform_);
}

void Elevator::SetWire(Wire* wire) {
	wire_ = wire;
}
