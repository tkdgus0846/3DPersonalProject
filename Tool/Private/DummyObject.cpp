#include "stdafx.h"
#include "DummyObject.h"
#include "GameInstance.h"

CDummyObject::CDummyObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummyObject::CDummyObject(const CDummyObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummyObject::Initialize_Prototype()
{
	/* 서버로 부터 받아와야할 데이터를 셋팅하낟. */
	/* 파일로부터  받아와야할 데이터를 셋팅하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::Initialize(void* pArg)
{
	/* 원형생성할 때 받아왔던 데이터 외의 추가적인 값들을 더 저장해주낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	return S_OK;
}

void CDummyObject::Tick(_double TimeDelta)
{
 	__super::Tick(TimeDelta);
}

void CDummyObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDERGROUP)m_eRenderGroup, this);
}

HRESULT CDummyObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pShaderCom != nullptr)
		m_pShaderCom->Begin(m_iPassNum);

	if (m_pVIBufferCom != nullptr)
		m_pVIBufferCom->Render();

	// 객체의 로컬축을 나타내자.

	return S_OK;
}

HRESULT CDummyObject::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		TEXT("Renderer"), (CComponent**)&m_pRendererCom, this)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC desc = CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f));
	if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Transform"),
		TEXT("Transform"), (CComponent**)&m_pTransformCom, this, &desc)))
		return E_FAIL;

	///* For.Com_VIBuffer */
	//if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	TEXT("VIBuffer"), (CComponent**)&m_pVIBufferCom, this)))
	//	return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	TEXT("Shader"), (CComponent**)&m_pShaderCom, this)))
	//	return E_FAIL;

	//
	///* For.Com_Texture */
	//if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
	//	TEXT("Texture"), (CComponent**)&m_pTextureCom, this)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::SetUp_ShaderResources()
{
	if (m_pTransformCom == nullptr ||
		m_pTextureCom == nullptr ||
		m_pRendererCom == nullptr ||
		m_pVIBufferCom == nullptr ||
		m_pShaderCom == nullptr) 
		return E_FAIL;

	_float4x4 myMatrix = m_pTransformCom->Get_WorldFloat4x4();
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &myMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/*XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 6.f, -1.f, 1.f), XMVectorSet(0.f, 0.f, 2.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.2f, 300.f));
*/
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	myMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", 
		&myMatrix)))
		return E_FAIL;

	myMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
		&myMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;
	
	/*_float3 lightPos = _float3(500.f, 500.f, 500.f);

	if (FAILED(m_pShaderCom->Bind_Float3("g_LightPosition", &lightPos)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}*/
	
	_float3 lightPos = _float3(500.f, 500.f, 500.f);
	m_pShaderCom->Bind_Float3("g_LightPosition", &lightPos);

	Safe_Release(pGameInstance);

	return S_OK;
}

CDummyObject * CDummyObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDummyObject*	pInstance = new CDummyObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummyObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject * CDummyObject::Clone(void * pArg)
{
	CDummyObject*	pInstance = new CDummyObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummyObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDummyObject::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	__super::Free();
}
