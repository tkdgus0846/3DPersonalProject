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
class CRandomChooser;
END

BEGIN(Client)

class CMonster : public CCreature
{
public:
	// 상태 패턴으로 몬스터의 행동을 결정한다.
	enum STATE
	{
		STATE_RUN, 
		STATE_WALK, 
		STATE_ATTACK,
		STATE_DAMAGED,
		STATE_KNOCKBACK,
		STATE_SUPERARMOR,
		STATE_STUN,
		STATE_IDLE,
		STATE_DEATH,
		STATE_END
	};

	struct MonsterState
	{
		_int	iPriority;
		STATE	eState;

		const bool operator < (const MonsterState& ms) const
		{
			return iPriority < ms.iPriority;
		}
	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Ready_BehaviorTree() PURE;

	virtual void	Tick(_float TimeDelta) override;
	virtual _int	Late_Tick(_float TimeDelta) override;
	virtual void	Select_AnimationKey() final {}; // 몬스터에선 이 함수 안쓸거임.

	virtual _bool	GetDead() override;

	/* 패트롤, 따라오기, 공격, 그리고 거리 멀어지면 대쉬로 따라오기*/
	CSelector* PaladinAI(const _float& MoveTime, const _float& WaitTime, _int AttackNums);
	/* AttackNums 랑 몬스터의 변수를 맞춰줘야한다. */
	CSelector* Patrol_Follow_AttackAI(const _float& PatrolMoveTime, const _float& WaitTime, _int AttackNums);


private: // 비해비어 트리 조립하는부분
	CSequence*		Make_RandomPatrol_AI(const _float& MoveTime, const _float& WaitTime);
	CSelector*		Make_FollowTarget_And_AttackAI_1();
	CSelector*		Make_FollowTarget_And_RandomAttackAI(_int AttackNums);
	CRandomChooser* Make_RandomAtatck_AI(_int AttackNums);
	class CTask_Attack*	Make_Attack_AI(_int AttackNum);

private: // 데코레이터 만드는 부분
	CDecorator*				Decorator_TargetOn();
	CDecorator*				Decorator_TargetOff();

	CDecorator*				Decorator_IsAttacking();
	CDecorator*				Decorator_IsNotAttacking();

	CDecorator*				Decorator_InDetectionRange();
	CDecorator*				Decorator_OutDetectionRange();

	CDecorator*	Decorator_TargetDistance_Farther_Than_StandardDistance();
	CDecorator* Decorator_TargetDistance_Closer_Than_StandardDistance();

	CDecorator*				Decorator_OutAttackRange();
	CDecorator*				Decorator_InAttackRange();

	CDecorator*				Decorator_IsAttackFinished(_int iAttackNum);
	CDecorator*				Decorator_IsNotAttackFinished(_int iAttackNum);

protected:		// 몬스터가 공통으로 당하거나 하는것들 ex) 넉백, 에어본 등등
	virtual void	Attack(const _float& TimeDelta) {}
	virtual void	Damaged(const _float& TimeDelta) {}
	virtual void	SuperArmor(const _float& TimeDelta) {}
	virtual void	Walk(const _float& TimeDelta) {}
	virtual void	Run(const _float& TimeDelta) {}
	virtual void	KnockBack(const _float& TimeDelta) {}
	virtual void	Stun(const _float& TimeDelta) {}
	virtual void	Idle(const _float& TimeDelta) {}
	virtual void	Death(const _float& TimeDelta) {}

	virtual void	State() {} // 이 State 함수에서 내 현재 상태를 정해줘야함.

	void			LookTarget();
	_vector			Compute_TargetPos();
	_float			Compute_TargetDistance();

	void			Add_State(STATE eState, _int iPriority);

private:
	void			Action_ByState(const _float& TimeDelta);

private:
	priority_queue<MonsterState> m_StateQueue;

protected:
	CBehaviorTree*	m_pBehaviorTreeCom = { nullptr };


	// AI 관련 변수들
	_bool					m_IsAttack1 = { false };
	_bool					m_IsAttack2 = { false };
	_bool					m_IsAttack3 = { false };
	_bool					m_IsAttack4 = { false };

	_bool					m_IsAttack1Started = { true };
	_bool					m_IsAttack2Started = { true };
	_bool					m_IsAttack3Started = { true };
	_bool					m_IsAttack4Started = { true };


	_bool					m_isWalk = { false };
	_bool					m_isRun = { false };

	// 객체에서 관리할 상태 변수들
	_bool					m_isDamaged = { false };
	_bool					m_isSuperArmor = { false };

	// 죽음 진행 변수들
	_bool					m_isDeathReserve = { false };
	_bool					m_isDeathFinished = { false };
	_float					m_DeathTimeAcc = { 0.f };
	const _float			m_DeathTime = { 5.f };

	// 피격과 슈퍼아머 변수들
	_float					m_DamagedStunTimeAcc = { 0.f };
	_float					m_SuperArmorTimeAcc = { 0.f };
	const _float			m_DamagedStunTime = { 0.5f }; // 스턴걸리는 시간 설정
	const _float			m_SuperArmorTime = { 4.f }; // 슈퍼아머 유지시간 설정


	// 이동 관련 변수들
	const _float			m_WalkSpeed = { 2.0f };
	const _float			m_RunSpeed = { 5.0f };

	// 넉백 관련 변수들
	_bool					m_bKnockBack = { false };
	const _float			m_KnockBackTime = { 2.f };
};
END

