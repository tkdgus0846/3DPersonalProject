#include "stdafx.h"
#include "..\Public\Horse.h"
#include "GameInstance.h"

CHorse::CHorse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CHorse::CHorse(const CHorse& rhs)
	: CCreature(rhs)
{
}

HRESULT CHorse::Initialize_Prototype()
{
	/* 서버로 부터 받아와야할 데이터를 셋팅하낟. */
	/* 파일로부터  받아와야할 데이터를 셋팅하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHorse::Initialize(void* pArg)
{
	/* 원형생성할 때 받아왔던 데이터 외의 추가적인 값들을 더 저장해주낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Add_Animations();

	m_pAnimInstance[HORSE_NATURAL]->Apply_Animation("Idle");

	m_pTransformCom->Set_Position({ 10.f,0.f,0.f,1.f });

	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	return S_OK;
}

void CHorse::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDERGROUP)m_eRenderGroup, this);
}

void CHorse::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CHorse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom[m_eCurHorseType]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom[m_eCurHorseType]->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom[m_eCurHorseType]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		// m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom[m_eCurHorseType]->Render(i);
	}

	return S_OK;
}

void CHorse::Add_Animations()
{
	AnimNode node;

	////////////// 맨손 애니메이션들 //////////////
	node.bLoop = true;

	node.AnimIndices = { 0 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("N_Walk", node);
	node.AnimIndices = { 1 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("NE_Walk", node);
	node.AnimIndices = { 2 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("SE_Walk", node);
	node.AnimIndices = { 3 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("S_Walk", node);
	node.AnimIndices = { 4 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("SW_Walk", node);
	node.AnimIndices = { 5 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("NW_Walk", node);
	node.AnimIndices = { 6 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("W_Walk", node);
	node.AnimIndices = { 7 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("E_Walk", node);
	node.AnimIndices = { 8 };
	m_pAnimInstance[HORSE_NATURAL]->Push_Animation("Idle", node);

	node.bLoop = false;

	node.AnimIndices = { 486, 487, 488 };
	node.eraseLessTime = { 0.05, 0.05, 0.04 };

}

HRESULT CHorse::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TRANSFORM_W, (CComponent**)&m_pTransformCom, &CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f)))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Horse_Mount"),
		L"Model_Mount", (CComponent**)&m_pModelCom[HORSE_MOUNT])))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Horse"),
		L"Model_Natural", (CComponent**)&m_pModelCom[HORSE_NATURAL])))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDINGBOX OBBDesc;

	OBBDesc.eColGroup = COL_ENEMY;
	OBBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	OBBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	OBBDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		COLLIDER_W, (CComponent**)&m_pColliderCom, &OBBDesc)))
		return E_FAIL;

	////////////////////////// 애님 인스턴스 ///////////////
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimInstance"), L"AnimInstance_Mount", (CComponent**)&m_pAnimInstance[HORSE_MOUNT], m_pModelCom[HORSE_MOUNT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimInstance"), L"AnimInstance_Natural", (CComponent**)&m_pAnimInstance[HORSE_NATURAL], m_pModelCom[HORSE_NATURAL])))
		return E_FAIL;

	return S_OK;
}

HRESULT CHorse::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
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

CHorse* CHorse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHorse* pInstance = new CHorse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHorse");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CHorse::Clone(void* pArg)
{
	CHorse* pInstance = new CHorse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHorse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHorse::Free()
{

	for (_int i = 0; i < HORSE_END; i++)
	{
		Safe_Release(m_pModelCom[i]);
		Safe_Release(m_pAnimInstance[i]);
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	

	__super::Free();
}
