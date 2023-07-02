#include "stdafx.h"
#include "..\Public\PlayerParts.h"
#include "GameInstance.h"

CPlayerParts::CPlayerParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActorComponent(pDevice, pContext)
{
}

CPlayerParts::CPlayerParts(const CPlayerParts& rhs)
	: CActorComponent(rhs)
{
}

HRESULT CPlayerParts::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CPlayerParts::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_WorldFloat4x4(m_ParentDesc.LocalOffsetMatrix);

	return S_OK;
}

void CPlayerParts::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	
}

void CPlayerParts::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CPlayerParts::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerParts::Add_Components()
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
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_ParentDesc.ModelName,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerParts::SetUp_ShaderResources()
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

CPlayerParts* CPlayerParts::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerParts* pInstance = new CPlayerParts(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayerParts");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerParts::Clone(void* pArg)
{
	CPlayerParts* pInstance = new CPlayerParts(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayerParts");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerParts::Free()
{
	__super::Free();
}
