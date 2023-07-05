#include "stdafx.h"
#include "..\Public\Monster.h"
#include "RootNode.h"
#include "Selector.h"
#include "Sequence.h"
#include "BlackBoard.h"
#include <Task_LookAt.h>
#include "Task_Wait.h"
#include "Decorator.h"
#include "Task_FollowTarget.h"
#include "Task_Attack1.h"
#include "BlackBoard.h"
#include "Task_RandomMove.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CCreature(rhs)
{
}

CSequence* CMonster::Make_RandomPatrol_AI(const _float& MoveTime, const _float& WaitTime)
{
	CSequence* PatrolSequence = CSequence::Create();

	PatrolSequence->Add_Decorator(Decorator_TargetOff());

	CTask_RandomMove* task1 = CTask_RandomMove::Create();
	CTask_Wait* task2 = CTask_Wait::Create();

	task1->Set_MoveTime(MoveTime);
	task2->Set_WaitTime(WaitTime);

	PatrolSequence->AddNode(task1);
	PatrolSequence->AddNode(task2);

	return PatrolSequence;
}

CSelector* CMonster::Make_FollowTarget_And_AttackAI()
{
	CSelector* selector = CSelector::Create();
	
	CTask_FollowTarget* followTargetTask = CTask_FollowTarget::Create();
	CTask_Attack1* attackTask = CTask_Attack1::Create();
	
	// 데코레이터를 정해주는 기준은 ~~게 되는가? 로 판단해야한다.
	selector->Add_Decorator(Decorator_TargetOn());
	
	followTargetTask->Add_Decorator(Decorator_OutAttackRange());
	attackTask->Add_Decorator(Decorator_InAttackRange());
	

	selector->AddNode(followTargetTask);
	selector->AddNode(attackTask);

	return selector;
}

CSelector* CMonster::Test(const _float& MoveTime, const _float& WaitTime)
{
	CSelector* selector = CSelector::Create();
	CSelector* FollowTargetSelector = Make_FollowTarget_And_AttackAI();
	CSequence* PatrolSequence = Make_RandomPatrol_AI(MoveTime, WaitTime);

	selector->AddNode(FollowTargetSelector);
	selector->AddNode(PatrolSequence);

	return selector;
}

CDecorator* CMonster::Decorator_TargetOn()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject* target = any_cast<CGameObject*>(pBlackBoard->FindData("Target"));

			return target != nullptr;
		}
	);
	
	return deco;
}

CDecorator* CMonster::Decorator_TargetOff()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			CGameObject* target = any_cast<CGameObject*>(pBlackBoard->FindData("Target"));

			return target == nullptr;
		}
	);

	return deco;
}

// 사거리 안으로 들어가면 Follow Target 을 멈춰야함. 따라서 bInAttackRange 가 true 이면 false 를 리턴 해야됨.
CDecorator* CMonster::Decorator_OutAttackRange()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool bInAttackRange = any_cast<_bool>(pBlackBoard->FindData("InAttackRange"));

			// 사거리 안에 들어오면 false 사거리 밖으로 나가면 true 를 뱉는다.
			return bInAttackRange == false;
		}
	);

	return deco;
}

CDecorator* CMonster::Decorator_InAttackRange()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool bInAttackRange = any_cast<_bool>(pBlackBoard->FindData("InAttackRange"));

			// 사거리 안에 들어오면 true 사거리 밖으로 나가면 true 를 뱉는다.
			return bInAttackRange;
		}
	);

	return deco;
}

