#pragma once

#include "Client_Defines.h"
#include "Creature.h"
#include "BehaviorTree.h"

BEGIN(Engine)
class CRootNode;
class CSelector;
class CSequence;
class CBlackBoard;
class CDecorator;
END

BEGIN(Client)
class CMonster : public CCreature
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

protected: // 비해비어 트리 조립하는부분
	CSequence*				Make_RandomPatrol_AI(const _float& MoveTime, const _float& WaitTime);
	CSelector*				Make_FollowTarget_And_AttackAI();
	//CSequence*				Make_IsRange_Attack_Sequence();
	//CSelector*				Make_Patrol_IsRange_Attack_Selector();

	CSelector* Test(const _float& MoveTime, const _float& WaitTime);

	//_bool					Is_Target_On();

	
protected: // 데코레이터 만드는 부분
	CDecorator*				Decorator_TargetOn();
	CDecorator*				Decorator_TargetOff();
	CDecorator*				Decorator_OutAttackRange();
	CDecorator*				Decorator_InAttackRange();

protected:
	CBehaviorTree*	m_pBehaviorTreeCom = { nullptr };
};
END

