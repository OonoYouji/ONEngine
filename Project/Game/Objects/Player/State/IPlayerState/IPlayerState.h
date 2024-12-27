#pragma once

#include <memory>

class IPlayerState {
public:

	IPlayerState(class Player* _player);
	virtual ~IPlayerState();

	virtual void Start()  = 0;
	virtual void Update() = 0;
	virtual void Exit()   = 0;

	virtual bool IsEnd()  = 0;
	virtual int NextStateIndex() = 0;

protected:

	class Player* pPlayer_ = nullptr;
};

