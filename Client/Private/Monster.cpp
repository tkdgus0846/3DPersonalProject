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


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CCreature(rhs)
{
}

CSequence* CMonster::Make_RandomPatrol_AI(const _double& MoveTime, const _double& WaitTime)
{
	return nullptr;
}

CSequence* CMonster::Make_FollowTarget_AI()
{
	CSequence* sequence = CSequence::Create();
	
	CTask_FollowTarget* followTargetTask = CTask_FollowTarget::Create();
	//followTargetTask->Add_Decorator(CDecorator::Create([=]))

	sequence->AddNode(followTargetTask);

	return sequence;
}

CSelector* CMonster::Test(const _double& RotTime, const _double& WaitTime)
{
	CSelector* selector = CSelector::Create();
	CSequence* FollowTargetSequence = Make_FollowTarget_AI();
	CSequence* TurnAndWaitSequence = CSequence::Create();
	
	CTask_LookAt* task1 = CTask_LookAt::Create();
	CTask_Wait* task2 = CTask_Wait::Create();
	
	task1->Set_TurnTime(RotTime);
	task2->Set_WaitTime(WaitTime);
	
	TurnAndWaitSequence->AddNode(task1);
	TurnAndWaitSequence->AddNode(task2);



	selector->AddNode(FollowTargetSequence);
	selector->AddNode(TurnAndWaitSequence);

	return selector;
}

