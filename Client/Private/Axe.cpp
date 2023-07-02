#include "stdafx.h"
#include "..\Public\Axe.h"
#include "GameInstance.h"
#include "Player.h"

CAxe::CAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{
}

CAxe::CAxe(const CAxe& rhs)
	: CWeapon(rhs)
{
}

HRESULT CAxe::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CAxe::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	// 여기에 오프셋 설정 같은걸 다시 해줘야한다.
	m_pTransformCom->Scaled(_float3(0.007f, 0.007f, 0.007f));
	//m_pTransformCom->Set_Position({ -2.f,0.f,0.f,1.f });
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.0f));

	///*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));*/

	return S_OK;
}

void CAxe::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	
}

void CAxe::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CAxe::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxe::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Axe_GEO #103351"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxe::SetUp_ShaderResources()
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

CAxe* CAxe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAxe* pInstance = new CAxe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAxe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAxe::Clone(void* pArg)
{
	CAxe* pInstance = new CAxe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAxe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAxe::Free()
{
	__super::Free();
}

void CAxe::Skill_Q(const _double& TimeDelta)
{
	if ((m_AxeDashInitSpeed + (m_AxeDashAccel * m_AxeDashTimeAcc)) < 0.f)
	{
		m_bAxeDashFinished = true;
		return;
	}

	m_AxeDashTimeAcc += TimeDelta;

	
	m_pPlayer->m_pTransformCom->Go_Dir(XMLoadFloat3(&m_DashDir), m_AxeDashInitSpeed, m_AxeDashAccel, m_AxeDashTimeAcc, m_pPlayer->m_pNavigationCom);
}

void CAxe::Skill_E(const _double& TimeDelta)
{
}

void CAxe::Skill_Q_Setting()
{
	XMStoreFloat3(&m_DashDir, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_AxeDashTimeAcc = 0.0;
	m_bAxeDashFinished = false;
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_Dash");
}

void CAxe::Skill_E_Setting()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_Throw");
}

_bool CAxe::Skill_Q_End()
{
	// 방향이 역방향이 되는 순간. 멈춰라.
	if (m_bAxeDashFinished == true)
		return true;
	return false;
}

_bool CAxe::Skill_E_End()
{
	return true;
}

void CAxe::N_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_N_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::NE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_NE_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::E_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_E_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::SE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_SE_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::S_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_S_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::SW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_SW_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::W_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_W_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::NW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_NW_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CAxe::Idle_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_Idle");
}

void CAxe::Equip_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_Equip");
}

void CAxe::Move_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_Right_MoveCast", ANIM_UPPERBODY);
}

void CAxe::Standing_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Axe_Right");
}
