#pragma once

#include "Client_Defines.h"
#include "Creature.h"
#include "BehaviorTree.h"

BEGIN(Engine)
class CRootNode;
class CSelector;
class CSequence;
class CBlackBoard;
END

BEGIN(Client)
class CMonster : public CCreature
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

protected:
	CSequence*				Make_RandomPatrol_AI(const _double& MoveTime, const _double& WaitTime);
	CSequence*				Make_FollowTarget_AI();
	//CSequence*				Make_IsRange_Attack_Sequence();
	//CSelector*				Make_Patrol_IsRange_Attack_Selector();

	CSelector* Test(const _double& RotTime, const _double& WaitTime);

	//_bool					Is_Target_On();
protected:
	CBehaviorTree* m_pBehaviorTreeCom = { nullptr };
};
END

