#include "..\Public\ActorComponent.h"
#include "GameInstance.h"

CActorComponent::CActorComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CActorComponent::CActorComponent(const CActorComponent & rhs)
	: CGameObject(rhs)
{

}

HRESULT CActorComponent::Initialize_Prototype()
{	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActorComponent::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memmove(&m_ParentDesc, pArg, sizeof m_ParentDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	

	return S_OK;
}

void CActorComponent::Tick(_double TimeDelta)
{
	if (m_bEnabled == false) return;

 	__super::Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

void CActorComponent::Late_Tick(_double TimeDelta)
{
	if (m_bEnabled == false) return;

	if (m_pModelCom == nullptr || m_pShaderCom == nullptr || m_pRendererCom == nullptr || m_pTransformCom == nullptr) 
		return;

	__super::Late_Tick(TimeDelta);

	_matrix BoneMatrix;


	/* 나의 월드행렬 * 부모(뼈의 오프셋 * 뼈의 컴바인드 * 피벗) */
	BoneMatrix = XMLoadFloat4x4(&m_ParentDesc.OffsetMatrix) *
		XMLoadFloat4x4(m_ParentDesc.pCombindTransformationMatrix) *
		XMLoadFloat4x4(&m_ParentDesc.PivotMatrix);

	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);


	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() *
		BoneMatrix * XMLoadFloat4x4(m_ParentDesc.pParentWorldMatrix));
}

HRESULT CActorComponent::Render()
{
	if (m_bEnabled == false) return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CActorComponent::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
