#include "stdafx.h"
#include "..\Public\CubeObject.h"
#include "GameInstance.h"

CCubeObject::CCubeObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCubeObject::CCubeObject(const CCubeObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCubeObject::Initialize_Prototype()
{
	/* 서버로 부터 받아와야할 데이터를 셋팅하낟. */
	/* 파일로부터  받아와야할 데이터를 셋팅하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCubeObject::Initialize(void* pArg)
{
	/* 원형생성할 때 받아왔던 데이터 외의 추가적인 값들을 더 저장해주낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/*m_pTransformCom->Set_Position({ 10.f,2.f,10.f,1.f });
	m_pTransformCom->Scaled({ 4.f,4.f,4.f });*/

	m_eRenderGroup = CRenderer::RENDER_PRIORITY;

	return S_OK;
}

void CCubeObject::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);
}

_int CCubeObject::Late_Tick(_float TimeDelta)
{
	_int result = __super::Late_Tick(TimeDelta);

	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pGameInstance->Get_CamPosition()));

	Safe_Release(pGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDERGROUP)m_eRenderGroup, this);

	return result;
}

HRESULT CCubeObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iPassNum);



	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCubeObject::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TRANSFORM_W, (CComponent**)&m_pTransformCom, &CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f)))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexCube"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Cube"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	//CBounding_OBB::BOUNDINGBOX OBBDesc;

	//OBBDesc.eColGroup = COL_ENEMY;
	//OBBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	//OBBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//OBBDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
	//	COLLIDER_W, (CComponent**)&m_pColliderCom, &OBBDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CCubeObject::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/*XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 6.f, -1.f, 1.f), XMVectorSet(0.f, 0.f, 2.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.2f, 300.f));
*/
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
		&pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		&pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	



	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CCubeObject* CCubeObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCubeObject* pInstance = new CCubeObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCubeObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CCubeObject::Clone(void* pArg)
{
	CCubeObject* pInstance = new CCubeObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCubeObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCubeObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);

	__super::Free();
}
