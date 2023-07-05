#pragma once

#include "Weapon.h"

BEGIN(Client)

class CSlasher final : public CWeapon
{
protected:
	CSlasher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlasher(const CSlasher& rhs);
	virtual ~CSlasher() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* ������ �����Ѵ�. */
	static CSlasher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

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
	// ������ Q ��ų ������
	_float3						m_DashDir;
	_float						m_SlasherDashTimeAcc = { 0.0 };
	const _float				m_SlasherDashAccel = { -4.1 };
	const _float				m_SlasherDashInitSpeed = { 1.41 };
	_float4						m_SlasherDashOriginPos;
	_bool						m_bSlasherDashFinished = { false };
	_bool						m_bSlasherDashStarted = { false };
};

END

