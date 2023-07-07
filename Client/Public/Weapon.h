#pragma once

#include "ActorComponent.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CWeapon : public CActorComponent
{
protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual void Skill_Q(const _float& TimeDelta)	PURE;
	virtual void Skill_E(const _float& TimeDelta)	PURE;
	virtual void Skill_Q_Setting()					PURE;
	virtual void Skill_E_Setting()					PURE;

	virtual _bool Skill_Q_End()						PURE;
	virtual _bool Skill_E_End()						PURE;

	virtual void SkillQ_Collision_Enter(const Collision* collision) {}
	virtual void SkillQ_Collision_Stay(const Collision* collision) {}
	virtual void SkillQ_Collision_Exit(const Collision* collision) {}

	virtual void SkillE_Collision_Enter(const Collision* collision) {}
	virtual void SkillE_Collision_Stay(const Collision* collision) {}
	virtual void SkillE_Collision_Exit(const Collision* collision) {}

	virtual void Attack_Collision_Enter(const Collision* collision);
	virtual void Attack_Collision_Stay(const Collision* collision);
	virtual void Attack_Collision_Exit(const Collision* collision);

	virtual void N_Walk_Animation()				PURE;
	virtual void NE_Walk_Animation()			PURE;
	virtual void E_Walk_Animation()				PURE;
	virtual void SE_Walk_Animation()			PURE;
	virtual void S_Walk_Animation()				PURE;
	virtual void SW_Walk_Animation()			PURE;
	virtual void W_Walk_Animation()				PURE;
	virtual void NW_Walk_Animation()			PURE;

	virtual void Idle_Animation()				PURE;
	virtual void Equip_Animation()				PURE;
	virtual void Move_Attack_Animation()		PURE;
	virtual void Standing_Attack_Animation()	PURE;


	void		EnableDashAttack() { m_bDashAttack = true; }
	void		SetPlayer() { m_pPlayer = (CPlayer*)m_pOwner; }
	_float		Get_AttackRate() const { return m_AttackRate; }
	_int		Get_Damage() const { return m_Damage; }

protected:
	_bool		m_bDashAttack = { false };
	_float		m_AttackRate = { 0.5 };
	_int		m_Damage = { 5 };
	_int		m_QDamage = { 1 };
	_int		m_EDamage = { 5 };

	class CPlayer* m_pPlayer = { nullptr };
};

END