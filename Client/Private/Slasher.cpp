#include "stdafx.h"
#include "..\Public\Slasher.h"
#include "GameInstance.h"
#include "Player.h"

CSlasher::CSlasher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon(pDevice, pContext)
{
}

CSlasher::CSlasher(const CSlasher& rhs)
	: CWeapon(rhs)
{
}

HRESULT CSlasher::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSlasher::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	// 여기에 오프셋 설정 같은걸 다시 해줘야한다.
	m_pTransformCom->Scaled(_float3(0.016f, 0.016f, 0.016f));
	//m_pTransformCom->Set_Position({ -2.f,0.f,0.f,1.f });
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));

	///*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));*/

	return S_OK;
}

void CSlasher::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	
}

void CSlasher::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CSlasher::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlasher::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SteelSlasher_2H_Standard_GEO"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlasher::SetUp_ShaderResources()
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

CSlasher* CSlasher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlasher* pInstance = new CSlasher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSlasher");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSlasher::Clone(void* pArg)
{
	CSlasher* pInstance = new CSlasher(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSlasher");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSlasher::Free()
{
	__super::Free();
}

void CSlasher::Skill_Q(const _float& TimeDelta)
{
	if (m_bSlasherDashFinished == true || m_bSlasherDashStarted == false)
		return;

	m_SlasherDashTimeAcc += TimeDelta;

	m_pPlayer->m_pTransformCom->Go_Dir(XMLoadFloat3(&m_DashDir), m_SlasherDashInitSpeed, m_SlasherDashAccel, m_SlasherDashTimeAcc, m_pPlayer->m_pNavigationCom);

	_vector vector1 = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vector2 = XMLoadFloat4(&m_SlasherDashOriginPos);

	_vector diff_vector = DirectX::XMVectorSubtract(vector2, vector1);
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(diff_vector));

	if (distance <= 1.2f)
	{
		m_bSlasherDashFinished = true;
		m_pPlayer->m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_SlasherDashOriginPos));
	}

}

void CSlasher::Skill_E(const _float& TimeDelta)
{
}

void CSlasher::Skill_Q_Setting()
{
	XMStoreFloat3(&m_DashDir, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	XMStoreFloat4(&m_SlasherDashOriginPos, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_SlasherDashTimeAcc = 0.0f;
	m_bSlasherDashFinished = false;
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_Q");
}

void CSlasher::Skill_E_Setting()
{
}

_bool CSlasher::Skill_Q_End()
{
	_bool bResult = false;
	m_bSlasherDashStarted = true;
	// 슬래셔 대쉬가 끝나면 멈춰라.
	if (m_bSlasherDashFinished == true)
	{
		bResult = true;
		m_bSlasherDashStarted = false;
	}
	return bResult;
}

_bool CSlasher::Skill_E_End()
{
	return _bool();
}

void CSlasher::N_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_N_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::NE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_NE_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::E_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_E_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::SE_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_SE_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::S_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_S_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::SW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_SW_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::W_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_W_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::NW_Walk_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_NW_Walk", m_pPlayer->m_bAttack == true ? ANIM_LOWERBODY : ANIM_ALLBODY);
}

void CSlasher::Idle_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_Idle");
}

void CSlasher::Equip_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_Equip");
}

void CSlasher::Move_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_Right", ANIM_UPPERBODY);
}

void CSlasher::Standing_Attack_Animation()
{
	m_pPlayer->m_pAnimInstance->Apply_Animation("Slasher_Right");
}
