#pragma once

#include "Weapon.h"

BEGIN(Client)

class CHand final : public CWeapon
{
protected:
	CHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHand(const CHand& rhs);
	virtual ~CHand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* ������ �����Ѵ�. */
	static CHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CWeapon��(��) ���� ��ӵ�
	virtual void Skill_Q(const _double& TimeDelta) override;
	virtual void Skill_E(const _double& TimeDelta) override;
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
	_double						m_MaceDashTimeAcc = { 0.0 };
	const _double				m_MaceDashAccel = { -0.01 };
	const _double				m_MaceDashInitSpeed = { 0.2 };

	const _double				m_MaceDashJumpSpeed = { 6.0 };
	const _double				m_MaceDashJumpGravity = { 10.5 };
	_double						m_MaceDashJumpOriginHeight = { 0.0 };

	_bool						m_bMaceDashFinished = { false };
};

END
