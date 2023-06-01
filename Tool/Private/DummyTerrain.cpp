#include "stdafx.h"
#include "DummyTerrain.h"
#include "GameInstance.h"

CDummyTerrain::CDummyTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummyTerrain::CDummyTerrain(const CDummyTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummyTerrain::Initialize_Prototype()
{
	/* ������ ���� �޾ƿ;��� �����͸� �����ϳ�. */
	/* ���Ϸκ���  �޾ƿ;��� �����͸� �����ϳ�. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyTerrain::Initialize(void* pArg)
{
	/* ���������� �� �޾ƿԴ� ������ ���� �߰����� ������ �� �������ֳ�. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	return S_OK;
}

void CDummyTerrain::Tick(_double TimeDelta)
{
 	__super::Tick(TimeDelta);
}

void CDummyTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CDummyTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);



	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CDummyTerrain::Add_Components()
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

	/* For.Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		TEXT("VIBuffer"), (CComponent**)&m_pVIBufferCom, this)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Shader"), (CComponent**)&m_pShaderCom, this)))
		return E_FAIL;

	
	/* For.Com_Texture */
	if (FAILED(Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Texture"), (CComponent**)&m_pTextureCom, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyTerrain::SetUp_ShaderResources()
{
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
	
	_float3 lightPos = _float3(500.f, 500.f, 500.f);

	if (FAILED(m_pShaderCom->Bind_Float3("g_LightPosition", &lightPos)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CDummyTerrain * CDummyTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDummyTerrain*	pInstance = new CDummyTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummyTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject * CDummyTerrain::Clone(void * pArg)
{
	CDummyTerrain*	pInstance = new CDummyTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummyTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDummyTerrain::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	__super::Free();
}
