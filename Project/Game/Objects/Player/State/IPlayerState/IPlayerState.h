#pragma once

class IPlayerState {
public:

	IPlayerState(class Player* _player);
	virtual ~IPlayerState();

	virtual void Start()  = 0;
	virtual void Update() = 0;
	virtual void End()    = 0;

protected:

	class Player* pPlayer_ = nullptr;
};

