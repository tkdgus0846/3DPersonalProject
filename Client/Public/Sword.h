#pragma once


#include "Weapon.h"

BEGIN(Client)

class CSword final : public CWeapon
{
	
protected:
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSword(const CSword& rhs);
	virtual ~CSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual _int Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* 원형을 생성한다. */
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본(실제 사용할 객체)을 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	virtual void SkillQ_Collision_Enter(const Collision* collision);
	virtual void SkillQ_Collision_Stay(const Collision* collision);
	virtual void SkillQ_Collision_Exit(const Collision* collision);

	
	virtual void SkillE_Collision_Enter(const Collision* collision);
	virtual void SkillE_Collision_Stay(const Collision* collision);
	virtual void SkillE_Collision_Exit(const Collision* collision);

	/*
	virtual void Attack_Collision_Enter(const Collision* collision);
	virtual void Attack_Collision_Stay(const Collision* collision);
	virtual void Attack_Collision_Exit(const Collision* collision);*/

	// CWeapon을(를) 통해 상속됨
	virtual void Skill_Q(const _float& TimeDelta) override;
	virtual void Skill_E(const _float& TimeDelta) override;
	virtual void Skill_Q_Setting() override;
	virtual void Skill_E_Setting() override;
	virtual _bool Skill_Q_End() override;
	virtual _bool Skill_E_End() override;
	virtual void N_Walk_Animation() override;
	virtual void NE_Walk_Animation() override;
	virtual void E_Walk_Animation() override;
	virtual void SE_Walk_Animation() override;
	virtual void S_Walk_Animation() override;
	virtual void SW_Walk_Animation() override;
	virtual void W_Walk_Animation() override;
	virtual void NW_Walk_Animation() override;
	virtual void Idle_Animation() override;
	virtual void Equip_Animation() override;
	virtual void Move_Attack_Animation() override;
	virtual void Standing_Attack_Animation() override;

private:
	// 도끼 Q 스킬 변수들
	_float3						m_DashDir;
	_float						m_SwordDashTimeAcc = { 0.0f };
	const _float				m_SwordDashAccel = { -4.1f };
	const _float				m_SwordDashInitSpeed = { 1.21f };
	_bool						m_bSwordDashFinished = { false };	

	_float						m_SwordSpinTimeAcc = { 0.0f };
	const _float				m_SwordSpinTime = { 5.f };
	_bool						m_bSwordSpinFinished = { false };
	const _float				m_SpinMoveSpeed = { 2.f };
	
	_float						m_SwordSpinTickTime = { 0.5f };
	_float						m_SwordSpinTickAcc = { 0.f };
};

END

