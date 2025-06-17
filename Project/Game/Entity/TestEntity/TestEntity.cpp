#include "TestEntity.h"

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Component.h"


void TestEntity::Initialize() {
	Script* script = AddComponent<Script>();
	script->SetScript("Player");
}

void TestEntity::Update() {

}
