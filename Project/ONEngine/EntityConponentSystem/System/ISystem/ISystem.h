#pragma once


class ISystem {
public:

	virtual ~ISystem() {}

	virtual void Update(size_t _entity, class ECSManager* _ecsManager) = 0;

};