#include "stdafx.h"
#include "..\Public\Mace.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Player_Main.h"
#include <Monster.h>

CMace::CMace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{
}

CMace::CMace(const CMace& rhs)
	: CWeapon(rhs)
{
}

HRESULT CMace::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMace::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_Damage = 10.f;

	// 여기에 오프셋 설정 같은걸 다시 해줘야한다.
	m_pTransformCom->Scaled(_float3(0.012f, 0.012f, 0.012f));
	//m_pTransformCom->Set_Position({ -2.f,0.f,0.f,1.f });
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));

	///*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));*/

	return S_OK;
}

void CMace::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	
}

void CMace::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMace::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMace::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f)))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mace_GEO"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMace::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		&pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		&pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition",
		&pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CMace* CMace::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMace* pInstance = new CMace(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMace");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMace::Clone(void* pArg)
{
	CMace* pInstance = new CMace(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMace");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMace::Free()
{
	__super::Free();
}

void CMace::Skill_Q(const _float& TimeDelta)
{
	if (m_bMaceDashFinished == true) return;

	m_MaceDashTimeAcc += TimeDelta;
	if ((m_MaceDashInitSpeed + (m_MaceDashAccel * m_MaceDashTimeAcc)) >= 0.f)
	{
		XMStoreFloat3(&m_DashDir, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		m_pPlayer->m_pTransformCom->Go_Dir(XMLoadFloat3(&m_DashDir), m_MaceDashInitSpeed, m_MaceDashAccel, m_MaceDashTimeAcc, m_pPlayer->m_pNavigationCom);
	}

	_float height = m_MaceDashJumpOriginHeight + (m_MaceDashJumpSpeed * m_MaceDashTimeAcc) - (0.5 * m_MaceDashJumpGravity * m_MaceDashTimeAcc * m_MaceDashTimeAcc);

	if (m_pPlayer->Compute_NavMesh_Height() <= height)
	{
		_vector myPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		myPos.m128_f32[1] = height;
		m_pPlayer->m_pTransformCom->Set_Position(myPos);

		m_pPlayer->m_bClimbNavMesh = false;
	}
	else
	{
		CCamera_Player_Main::MAINCAMERASHAKE desc;
		desc.fShakeMagnitude = 0.6f;
		CGameInstance::GetInstance()->On_Shake(&desc);
		m_pPlayer->m_bClimbNavMesh = true;
		m_bMaceDashFinished = true;
	}
}

void CMace::Skill_E(const _float& TimeDelta)
{
	m_MaceDashTimeAcc += TimeDelta;
	if ((m_MaceSpinInitSpeed + (m_MaceSpinAccel * m_MaceDashTimeAcc)) >= 0.f)
	{
		XMStoreFloat3(&m_DashDir, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		m_pPlayer->m_pTransformCom->Go_Dir(XMLoadFloat3(&m_DashDir), m_MaceSpinInitSpeed, m_MaceSpinAccel, m_MaceDashTimeAcc, m_pPlayer->m_pNavigationCom);
	}
	else
	{
		m_bMaceSpinFinished = true;
	}
}

void CMace::Skill_Q_Setting()
{
	XMStoreFloat3(&m_DashDir, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_MaceDashJumpOriginHeight = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
	m_MaceDashTimeAcc = 0.0f;
	//m_bSlasherDashFinished = false;
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_Dash");
}

void CMace::Skill_E_Setting()
{
	m_pPlayer->Collider_On(CPlayer::COLLIDER_MACE_E);

	XMStoreFloat3(&m_DashDir, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_MaceDashTimeAcc = 0.0f;
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_Spin");
}

_bool CMace::Skill_Q_End()
{
	_bool bResult = false;
	if (m_bMaceDashFinished == true)
	{
		bResult = true;
		m_bMaceDashFinished = false;
	}
	return bResult;
}

_bool CMace::Skill_E_End()
{
	_bool bResult = false;
	if (m_bMaceSpinFinished == true)
	{
		bResult = true;
		m_bMaceSpinFinished = false;
		m_pPlayer->Collider_Off(CPlayer::COLLIDER_MACE_E);
	}
	return bResult;
}

void CMace::N_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_N_Walk");
}

void CMace::NE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_NE_Walk");
}

void CMace::E_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_E_Walk");
}

void CMace::SE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_SE_Walk");
}

void CMace::S_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_S_Walk");
}

void CMace::SW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_SW_Walk");
}

void CMace::W_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_W_Walk");
}

void CMace::NW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_NW_Walk");
}

void CMace::Idle_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_Idle");
}

void CMace::Equip_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_Equip");
}

void CMace::Move_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_Right");
}

void CMace::Standing_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Mace_Right");
}

void CMace::SkillQ_Collision_Enter(const Collision* collision)
{
}

void CMace::SkillQ_Collision_Stay(const Collision* collision)
{
}

void CMace::SkillQ_Collision_Exit(const Collision* collision)
{
}

void CMace::SkillE_Collision_Enter(const Collision* collision)
{
	
	
}

void CMace::SkillE_Collision_Stay(const Collision* collision)
{
	CMonster* monster = (CMonster*)collision->OtherGameObject;
	CCollider* collider = m_pPlayer->m_pColliderCom[CPlayer::COLLIDER_MACE_E];

	if (monster != nullptr && collider == collision->MyCollider)
	{
		_vector vLookDir = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		CTransform* monsterTransform = (CTransform*)monster->Get_Component(TRANSFORM_W);

		if (monsterTransform == nullptr) return;

		_vector vPos = monsterTransform->Get_State(CTransform::STATE_POSITION);
		monsterTransform->Go_Dir(vPos + vLookDir, 0.015f, nullptr);

	}
}



void CMace::SkillE_Collision_Exit(const Collision* collision)
{
}
