#include "stdafx.h"
#include "..\Public\Axe2.h"
#include "GameInstance.h"

CAxe2::CAxe2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActorComponent(pDevice, pContext)
{
}

CAxe2::CAxe2(const CAxe2& rhs)
	: CActorComponent(rhs)
{
}

HRESULT CAxe2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CAxe2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	// ���⿡ ������ ���� ������ �ٽ� ������Ѵ�.
	m_pTransformCom->Scaled(_float3(0.007f, 0.007f, 0.007f));
	//m_pTransformCom->Set_Position({ -2.f,0.f,0.f,1.f });
	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.0f));

	///*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));*/

	return S_OK;
}

void CAxe2::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	
}

void CAxe2::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CAxe2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxe2::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Axe_GEO #103351_2"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxe2::SetUp_ShaderResources()
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

CAxe2* CAxe2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAxe2* pInstance = new CAxe2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAxe2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAxe2::Clone(void* pArg)
{
	CAxe2* pInstance = new CAxe2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAxe2");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAxe2::Free()
{
	__super::Free();
}
