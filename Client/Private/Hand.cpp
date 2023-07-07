#include "stdafx.h"
#include "..\Public\Hand.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Player_Main.h"

CHand::CHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{
}

CHand::CHand(const CHand& rhs)
	: CWeapon(rhs)
{
}

HRESULT CHand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHand::Initialize(void* pArg)
{
	return S_OK;
}

void CHand::Tick(_float TimeDelta)
{
}

_int CHand::Late_Tick(_float TimeDelta)
{
	return __super::Late_Tick(TimeDelta);
}

HRESULT CHand::Render()
{
	return S_OK;
}

HRESULT CHand::Add_Components()
{
	return S_OK;
}

HRESULT CHand::SetUp_ShaderResources()
{
	return S_OK;
}

CHand* CHand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHand* pInstance = new CHand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHand::Clone(void* pArg)
{
	CHand* pInstance = new CHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHand::Free()
{
	__super::Free();
}

void CHand::Skill_Q(const _float& TimeDelta)
{

}

void CHand::Skill_E(const _float& TimeDelta)
{
}

void CHand::Skill_Q_Setting()
{
}

void CHand::Skill_E_Setting()
{
}

_bool CHand::Skill_Q_End()
{
	return true;
}

_bool CHand::Skill_E_End()
{
	return true;
}

void CHand::N_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("N_Walk");
}

void CHand::NE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("NE_Walk");
}

void CHand::E_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("E_Walk");
}

void CHand::SE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("SE_Walk");
}

void CHand::S_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("S_Walk");
}

void CHand::SW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("SW_Walk");
}

void CHand::W_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("W_Walk");
}

void CHand::NW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("NW_Walk");
}

void CHand::Idle_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Idle");
}

void CHand::Equip_Animation()
{
}

void CHand::Move_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Claw_Right");
}

void CHand::Standing_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Claw_Right_Standing");
}
