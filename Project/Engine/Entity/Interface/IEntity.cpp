#include "IEntity.h"

IEntity::IEntity() {
	transform_ = AddComponent<Transform>();
}
