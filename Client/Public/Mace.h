#pragma once

#include "Weapon.h"

BEGIN(Client)

class CMace final : public CWeapon
{
protected:
	CMace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMace(const CMace& rhs);
	virtual ~CMace() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual _int Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* ������ �����Ѵ�. */
	static CMace* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	virtual void SkillQ_Collision_Enter(const Collision* collision);
	virtual void SkillQ_Collision_Stay(const Collision* collision);
	virtual void SkillQ_Collision_Exit(const Collision* collision);

	virtual void SkillE_Collision_Enter(const Collision* collision);
	virtual void SkillE_Collision_Stay(const Collision* collision);
	virtual void SkillE_Collision_Exit(const Collision* collision);

	// CWeapon��(��) ���� ��ӵ�
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
	// ���̽� Q ��ų ������
	_float3						m_DashDir;
	_float						m_MaceDashTimeAcc = { 0.0f };
	const _float				m_MaceDashAccel = { -0.01f };
	const _float				m_MaceDashInitSpeed = { 0.2f };
	_bool						m_bMaceDashFinished = { false };
	const _float				m_MaceDashJumpSpeed = { 6.0f };
	const _float				m_MaceDashJumpGravity = { 10.5f };
	_float						m_MaceDashJumpOriginHeight = { 0.0f };

	// ���̽� E ��ų ������
	const _float				m_MaceSpinAccel = { -0.7f };
	const _float				m_MaceSpinInitSpeed = { 0.3f };
	_bool						m_bMaceSpinFinished = { false };
	

	
};

END

