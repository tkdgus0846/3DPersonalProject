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
#include "Task_Attack.h"
#include "BlackBoard.h"
#include "Task_RandomMove.h"
#include "RandomChooser.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CCreature(rhs)
{
}

HRESULT CMonster::Initialize(void* pArg)
{
	HRESULT hr = __super::Initialize(pArg);

	if (FAILED(Ready_BehaviorTree()))
		return E_FAIL;

	return hr;
}

void CMonster::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	State(); // 상태 큐들을 집어넣어주고
	Action_ByState(TimeDelta); // 처리하고 지워주고

}

_int CMonster::Late_Tick(_float TimeDelta)
{
	return __super::Late_Tick(TimeDelta);

}

_bool CMonster::GetDead()
{
	m_isDeathReserve = __super::GetDead();

	return  m_isDeathFinished;
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

CSelector* CMonster::Make_FollowTarget_And_AttackAI_1()
{
	CSelector* selector = CSelector::Create();
	
	CTask_FollowTarget* followTargetTask = CTask_FollowTarget::Create();
	CTask_Attack* attackTask = CTask_Attack::Create("IsAttack1");
	
	// 데코레이터를 정해주는 기준은 ~~게 되는가? 로 판단해야한다.
	selector->Add_Decorator(Decorator_TargetOn());
	
	followTargetTask->Add_Decorator(Decorator_OutAttackRange());
	attackTask->Add_Decorator(Decorator_InAttackRange());
	
	selector->AddNode(followTargetTask);
	selector->AddNode(attackTask);

	return selector;
}

CSelector* CMonster::Make_FollowTarget_And_RandomAttackAI(_int AttackNums)
{
	CSelector* selector = CSelector::Create();

	CTask_FollowTarget* followTargetTask = CTask_FollowTarget::Create();

	// 데코레이터를 정해주는 기준은 ~~게 되는가? 로 판단해야한다.
	selector->Add_Decorator(Decorator_TargetOn());

	// 공격 중이 아닐때, 공격 사거리 밖일때 노드를 실행하게 한다.
	followTargetTask->Add_Decorator(Decorator_IsNotAttacking());
	followTargetTask->Add_Decorator(Decorator_OutAttackRange());

	selector->AddNode(followTargetTask);
	selector->AddNode(Make_RandomAtatck_AI(AttackNums));

	return selector;
}

CRandomChooser* CMonster::Make_RandomAtatck_AI(_int AttackNums)
{
	CRandomChooser* RandomChooser = CRandomChooser::Create();

	RandomChooser->Add_Decorator(Decorator_InAttackRange());

	for (_int num = 1; num <= AttackNums; num++)
	{
		CTask_Attack* taskAttack = CTask_Attack::Create("IsAttack"+to_string(num));
		taskAttack->Add_Decorator(Decorator_IsNotAttackFinished(num));
		RandomChooser->AddNode(taskAttack);
	}
	return RandomChooser;
}

CTask_Attack* CMonster::Make_Attack_AI(_int AttackNum)
{
	CTask_Attack* taskAttack = CTask_Attack::Create("IsAttack" + to_string(AttackNum));

	return taskAttack;
}


CSelector* CMonster::PaladinAI(const _float& MoveTime, const _float& WaitTime, _int AttackNums)
{
	CSelector* PaladinSelector = CSelector::Create();

	CTask_Attack* DashAttack = Make_Attack_AI(AttackNums + 1);
	CSelector* FollowRandomAttackSelector = Make_FollowTarget_And_RandomAttackAI(AttackNums);
	CSequence* patrolSequence = Make_RandomPatrol_AI(MoveTime, WaitTime);

	DashAttack->Add_Decorator(CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_float TargetDistance = any_cast<_float>(pBlackBoard->FindData("TargetDistance"));
			_float StandardDistance = any_cast<_float>(pBlackBoard->FindData("StandardDistance"));
			_bool bIsAttackStarted = any_cast<_bool>(pBlackBoard->FindData("IsAttack4Started"));

			/*cout << "TargetDistance: " << TargetDistance << " StandardDistance: " << StandardDistance << endl;*/
			// 공격 중이 아니라면 true.
			return (StandardDistance <= TargetDistance) || !bIsAttackStarted;
		}
	));

	DashAttack->Add_Decorator(Decorator_IsNotAttackFinished(AttackNums + 1));
	DashAttack->Add_Decorator(Decorator_TargetOn());
	//DashAttack->Add_Decorator(Decorator_TargetDistance_Farther_Than_StandardDistance());

	FollowRandomAttackSelector->Add_Decorator(Decorator_TargetDistance_Closer_Than_StandardDistance());
	
	PaladinSelector->AddNode(DashAttack);
	PaladinSelector->AddNode(FollowRandomAttackSelector);
	PaladinSelector->AddNode(patrolSequence);

	return PaladinSelector;
}

CSelector* CMonster::Patrol_Follow_AttackAI(const _float& PatrolMoveTime, const _float& WaitTime, _int AttackNums)
{
	CSelector* selector = CSelector::Create();
	CSelector* FollowTargetAttackSelector = Make_FollowTarget_And_RandomAttackAI(AttackNums);
	CSequence* PatrolSequence = Make_RandomPatrol_AI(PatrolMoveTime, WaitTime);

	selector->AddNode(FollowTargetAttackSelector);
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

CDecorator* CMonster::Decorator_IsAttacking()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool bIsAttacking = any_cast<_bool>(pBlackBoard->FindData("IsAttacking"));

			// 공격 중이라면 true
			return bIsAttacking == true;
		}
	);
	return deco;
}

CDecorator* CMonster::Decorator_IsNotAttacking()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool bIsAttacking = any_cast<_bool>(pBlackBoard->FindData("IsAttacking"));

			// 공격 중이 아니라면 true.
			return bIsAttacking == false;
		}
	);
	return deco;
}

CDecorator* CMonster::Decorator_InDetectionRange()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool InDetectionRange = any_cast<_bool>(pBlackBoard->FindData("InDetectionRange"));

			// 공격 중이 아니라면 true.
			return InDetectionRange == true;
		}
	);
	return deco;
}

CDecorator* CMonster::Decorator_OutDetectionRange()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool InDetectionRange = any_cast<_bool>(pBlackBoard->FindData("InDetectionRange"));

			// 공격 중이 아니라면 true.
			return InDetectionRange == false;
		}
	);
	return deco;
}

CDecorator* CMonster::Decorator_TargetDistance_Farther_Than_StandardDistance()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_float TargetDistance = any_cast<_float>(pBlackBoard->FindData("TargetDistance"));
			_float StandardDistance = any_cast<_float>(pBlackBoard->FindData("StandardDistance"));

			/*cout << "TargetDistance: " << TargetDistance << " StandardDistance: " << StandardDistance << endl;*/
			// 공격 중이 아니라면 true.
			return StandardDistance <= TargetDistance;
		}
	);
	return deco;
}

CDecorator* CMonster::Decorator_TargetDistance_Closer_Than_StandardDistance()
{
	CDecorator* deco = CDecorator::Create
	(
		[&](CBlackBoard* pBlackBoard)->_bool
		{
			_float TargetDistance = any_cast<_float>(pBlackBoard->FindData("TargetDistance"));
			_float StandardDistance = any_cast<_float>(pBlackBoard->FindData("StandardDistance"));


			// 공격 중이 아니라면 true.
			return StandardDistance >= TargetDistance;
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

CDecorator* CMonster::Decorator_IsAttackFinished(_int iAttackNum)
{
	string findStr = "IsAttack" + to_string(iAttackNum) + "Finished";
	
	CDecorator* deco = CDecorator::Create
	(
		[=](CBlackBoard* pBlackBoard)->_bool
		{
			_bool bIsAttackFinished = any_cast<_bool>(pBlackBoard->FindData(findStr));

			// 사거리 안에 들어오면 true 사거리 밖으로 나가면 true 를 뱉는다.
			return bIsAttackFinished;
		}
	);

	return deco;
}

CDecorator* CMonster::Decorator_IsNotAttackFinished(_int iAttackNum)
{
	string findStr = "IsAttack" + to_string(iAttackNum) + "Finished";

	CDecorator* deco = CDecorator::Create
	(
		[=](CBlackBoard* pBlackBoard)->_bool
		{
			_bool bIsAttackFinished = any_cast<_bool>(pBlackBoard->FindData(findStr));

			// 사거리 안에 들어오면 true 사거리 밖으로 나가면 true 를 뱉는다.
			return !bIsAttackFinished;
		}
	);

	return deco;
}

///////////////////////////////////////////////////////////////////////////
//////////////////////// 아래는 인공지능과는 별개의 공통된 동작들이다.
///////////////////////////////////////////////////////////////////////////

void CMonster::LookTarget()
{
	CGameObject* pTarget = any_cast<CGameObject*>(m_pBehaviorTreeCom->GetData("Target"));
	if (pTarget == nullptr) return;

	CTransform* targetTransform = (CTransform*)pTarget->Get_Component(TRANSFORM_W);
	if (targetTransform == nullptr) return;

	CTransform* myTransform = (CTransform*)Get_Component(TRANSFORM_W);
	if (myTransform == nullptr) return;

	myTransform->LookAt(targetTransform->Get_State(CTransform::STATE_POSITION), CTransform::LANDOBJECT);
}

_vector CMonster::Compute_TargetPos()
{
	CGameObject* pTarget = any_cast<CGameObject*>(m_pBehaviorTreeCom->GetData("Target"));
	if (pTarget == nullptr) return {};

	CTransform* targetTransform = (CTransform*)pTarget->Get_Component(TRANSFORM_W);
	if (targetTransform == nullptr) return {};

	return targetTransform->Get_State(CTransform::STATE_POSITION);
}

_float CMonster::Compute_TargetDistance()
{
	CGameObject* pTarget = any_cast<CGameObject*>(m_pBehaviorTreeCom->GetData("Target"));
	if (pTarget == nullptr) return -10.f;

	CTransform* targetTransform = (CTransform*)pTarget->Get_Component(TRANSFORM_W);
	if (targetTransform == nullptr) return -10.f;

	CTransform* myTransform = (CTransform*)Get_Component(TRANSFORM_W);
	if (myTransform == nullptr) return -10.f;

	_vector targetPos = targetTransform->Get_State(CTransform::STATE_POSITION);
	_vector myPos = myTransform->Get_State(CTransform::STATE_POSITION);

	return XMVector3Length(myPos - targetPos).m128_f32[0];
}

void CMonster::Add_State(STATE eState, _int iPriority)
{
	m_StateQueue.push({ iPriority,eState });
}

void CMonster::Action_ByState(const _float& TimeDelta)
{
	while (!m_StateQueue.empty())
	{
		MonsterState state = m_StateQueue.top();
		m_StateQueue.pop();

		switch (state.eState)
		{
		case STATE_RUN:
			Run(TimeDelta);
			break;
		case STATE_WALK:
			Walk(TimeDelta);
			break;
		case STATE_DAMAGED:
			Damaged(TimeDelta);
			break;
		case STATE_ATTACK:
			Attack(TimeDelta);
			break;
		case STATE_KNOCKBACK:
			KnockBack(TimeDelta);
			break;
		case STATE_SUPERARMOR:
			SuperArmor(TimeDelta);
			break;
		case STATE_STUN:
			Stun(TimeDelta);
			break;
		case STATE_IDLE:
			Idle(TimeDelta);
			break;
		case STATE_DEATH:
			Death(TimeDelta);
			break;
		default:
			break;
		}
	}
	
}

