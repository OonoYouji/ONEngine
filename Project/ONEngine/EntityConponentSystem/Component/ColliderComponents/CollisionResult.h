#pragma once
#include <functional>

struct CollisionResult {
	std::function<void(size_t _collisionEntity)> onCollisionEnter;
	std::function<void(size_t _collisionEntity)> onCollisionStay;
	std::function<void(size_t _collisionEntity)> onCollisionExit;

	std::vector<size_t> enterCollideds_;
	std::vector<size_t> stayCollideds_;
	std::vector<size_t> exitCollideds_;
};