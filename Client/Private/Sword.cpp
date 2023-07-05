#include "stdafx.h"
#include "..\Public\Sword.h"
#include "GameInstance.h"
#include "Player.h"

CSword::CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{
}

CSword::CSword(const CSword& rhs)
	: CWeapon(rhs)
{
}

HRESULT CSword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSword::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	// 여기에 오프셋 설정 같은걸 다시 해줘야한다.
	m_pTransformCom->Scaled(_float3(0.011f, 0.011f, 0.011f));
	//m_pTransformCom->Set_Position({ -2.f,0.f,0.f,1.f });
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.0f));

	///*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));*/

	return S_OK;
}

void CSword::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	
}

void CSword::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CSword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sword_GEO"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::SetUp_ShaderResources()
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

CSword* CSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSword* pInstance = new CSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSword");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSword::Clone(void* pArg)
{
	CSword* pInstance = new CSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSword");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSword::Free()
{
	__super::Free();
}

// 스킬 Q 가 활성화되어잇을대 틱에서 처리할것
void CSword::Skill_Q(const _float& TimeDelta)
{
	if ((m_SwordDashInitSpeed + (m_SwordDashAccel * m_SwordDashTimeAcc)) < 0.f)
	{
		m_bSwordDashFinished = true;
		return;
	}

	m_SwordDashTimeAcc += TimeDelta;

	
	m_pPlayer->m_pTransformCom->Go_Dir(XMLoadFloat3(&m_DashDir), m_SwordDashInitSpeed, m_SwordDashAccel, m_SwordDashTimeAcc, m_pPlayer->m_pNavigationCom);
}

void CSword::Skill_E(const _float& TimeDelta)
{
	if (m_SwordSpinTime <= m_SwordSpinTimeAcc)
	{
		m_bSwordSpinFinished = true;
		return;
	}
	m_SwordSpinTimeAcc += TimeDelta;
	
}

// q 스킬을 시작할때 세팅
void CSword::Skill_Q_Setting()
{
	XMStoreFloat3(&m_DashDir, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_SwordDashTimeAcc = 0.0f;
	m_bSwordDashFinished = false;
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_Dash");
}

void CSword::Skill_E_Setting()
{
	m_SwordSpinTimeAcc = 0.f;
	m_bSwordSpinFinished = false;
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_Spin");
	m_pPlayer->m_pTransformCom->Change_Speed(m_SpinMoveSpeed);
}

// q 스킬이 끝나는 시점 bool 리턴
_bool CSword::Skill_Q_End()
{
	// 방향이 역방향이 되는 순간. 멈춰라.
	if (m_bSwordDashFinished == true)
		return true;
	return false;
}

_bool CSword::Skill_E_End()
{
	if (m_bSwordSpinFinished == true)
		return true;
	return false;
}

void CSword::N_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_N_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::NE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_NE_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::E_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_E_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::SE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_SE_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::S_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_S_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::SW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_SW_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::W_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_W_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::NW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_NW_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSword::Idle_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_Idle");
}

void CSword::Equip_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_Equip");
}

void CSword::Move_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_Right_MoveCast", ANIM_UPPERBODY);
}

void CSword::Standing_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Sword_Right");
}
