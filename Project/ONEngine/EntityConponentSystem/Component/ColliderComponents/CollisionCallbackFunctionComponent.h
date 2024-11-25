#pragma once

#include <functional>

struct CollisionCallbackFunctionComponent {
	std::function<void(size_t _collisionEntity)> onCollisionEnter;
	std::function<void(size_t _collisionEntity)> onCollisionStay;
	std::function<void(size_t _collisionEntity)> onCollisionExit;
};