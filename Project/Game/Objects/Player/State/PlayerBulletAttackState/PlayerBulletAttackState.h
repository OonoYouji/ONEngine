#pragma once

/// std
#include <list>

#include "../IPlayerState/IPlayerState.h"


class PlayerBulletAttackState : public IPlayerState {
public:
	
	PlayerBulletAttackState(class Player* _player, class PlayerStateManager* _stateManager);
	~PlayerBulletAttackState();
	
	void Start()  override;
	void Update() override;
	void Exit()   override;
	
	bool IsEnd() override;
	int NextStateIndex() override;
	
	void Debug() override;


	void CreateBullets();

	void FireBullets();

	/// <summary>
	/// 左右に弾を広げる
	/// </summary>
	void SpreadToTheLeftAndRight();

private:

	float maxTime_;
	float currentTime_;

	int creationBulletNum_;

	std::list<class PlayerBullet*> bullets_;
	std::list<class Enemy*> forwardEnemyList_;

	std::list<class PlayerBullet*> leftBulletList_;
	std::list<class PlayerBullet*> rightBulletList_;


	bool isEnd_;
};
