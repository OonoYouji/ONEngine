#include "IEnemyState.h"
#include "Game/Objects/Enemy/Enemy.h"

#include "EnemyAttackState.h"
#include "EnemyIdleState.h"

IEnemyState::IEnemyState(Enemy* enemy): enemy_(enemy){}
