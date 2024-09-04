#include "Wire.h"

#include <ModelManager.h>


void Wire::Initialize() {
	model_ = ModelManager::Load("Wire");
}

void Wire::Update() {

}

void Wire::Draw() {
	model_->Draw(&transform_);
}
