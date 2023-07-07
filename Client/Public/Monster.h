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
	// ���� �������� ������ �ൿ�� �����Ѵ�.
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
	virtual void	Select_AnimationKey() final {}; // ���Ϳ��� �� �Լ� �Ⱦ�����.

	virtual _bool	GetDead() override;

	/* ��Ʈ��, �������, ����, �׸��� �Ÿ� �־����� �뽬�� �������*/
	CSelector* PaladinAI(const _float& MoveTime, const _float& WaitTime, _int AttackNums);
	/* AttackNums �� ������ ������ ��������Ѵ�. */
	CSelector* Patrol_Follow_AttackAI(const _float& PatrolMoveTime, const _float& WaitTime, _int AttackNums);


private: // ���غ�� Ʈ�� �����ϴºκ�
	CSequence*		Make_RandomPatrol_AI(const _float& MoveTime, const _float& WaitTime);
	CSelector*		Make_FollowTarget_And_AttackAI_1();
	CSelector*		Make_FollowTarget_And_RandomAttackAI(_int AttackNums);
	CRandomChooser* Make_RandomAtatck_AI(_int AttackNums);
	class CTask_Attack*	Make_Attack_AI(_int AttackNum);

private: // ���ڷ����� ����� �κ�
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

protected:		// ���Ͱ� �������� ���ϰų� �ϴ°͵� ex) �˹�, ��� ���
	virtual void	Attack(const _float& TimeDelta) {}
	virtual void	Damaged(const _float& TimeDelta) {}
	virtual void	SuperArmor(const _float& TimeDelta) {}
	virtual void	Walk(const _float& TimeDelta) {}
	virtual void	Run(const _float& TimeDelta) {}
	virtual void	KnockBack(const _float& TimeDelta) {}
	virtual void	Stun(const _float& TimeDelta) {}
	virtual void	Idle(const _float& TimeDelta) {}
	virtual void	Death(const _float& TimeDelta) {}

	virtual void	State() {} // �� State �Լ����� �� ���� ���¸� ���������.

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


	// AI ���� ������
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

	// ��ü���� ������ ���� ������
	_bool					m_isDamaged = { false };
	_bool					m_isSuperArmor = { false };

	// ���� ���� ������
	_bool					m_isDeathReserve = { false };
	_bool					m_isDeathFinished = { false };
	_float					m_DeathTimeAcc = { 0.f };
	const _float			m_DeathTime = { 5.f };

	// �ǰݰ� ���۾Ƹ� ������
	_float					m_DamagedStunTimeAcc = { 0.f };
	_float					m_SuperArmorTimeAcc = { 0.f };
	const _float			m_DamagedStunTime = { 0.5f }; // ���ϰɸ��� �ð� ����
	const _float			m_SuperArmorTime = { 4.f }; // ���۾Ƹ� �����ð� ����


	// �̵� ���� ������
	const _float			m_WalkSpeed = { 2.0f };
	const _float			m_RunSpeed = { 5.0f };

	// �˹� ���� ������
	_bool					m_bKnockBack = { false };
	const _float			m_KnockBackTime = { 2.f };
};
END

