#pragma once


#include "Weapon.h"

BEGIN(Client)

class CAxe final : public CWeapon
{
	
protected:
	CAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAxe(const CAxe& rhs);
	virtual ~CAxe() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	/* 원형을 생성한다. */
	static CAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본(실제 사용할 객체)을 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

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
	_float						m_AxeDashTimeAcc = { 0.0f };
	const _float				m_AxeDashAccel = { -4.1f };
	const _float				m_AxeDashInitSpeed = { 1.21f };
	_bool						m_bAxeDashFinished = { false };	
};

END

