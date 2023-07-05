#include "stdafx.h"
#include "..\Public\Paladin.h"
#include "GameInstance.h"
#include "RootNode.h"
#include "BlackBoard.h"
#include <Player.h>

CPaladin::CPaladin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CPaladin::CPaladin(const CPaladin& rhs)
	: CMonster(rhs)
{
}

HRESULT CPaladin::Initialize_Prototype()
{
	/* 서버로 부터 받아와야할 데이터를 셋팅하낟. */
	/* 파일로부터  받아와야할 데이터를 셋팅하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPaladin::Initialize(void* pArg)
{
	/* 원형생성할 때 받아왔던 데이터 외의 추가적인 값들을 더 저장해주낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Add_Animations();

	m_pAnimInstance->Apply_Animation("Idle");

	m_pTransformCom->Set_Position({ 10.f,0.f,5.f,1.f });
	m_pTransformCom->Change_Speed(m_WalkSpeed);

	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	return S_OK;
}

void CPaladin::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	State();
	Damaged(TimeDelta);
	SuperArmor(TimeDelta);

	Select_AnimationKey();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDERGROUP)m_eRenderGroup, this);
}

void CPaladin::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (m_pRendererCom != nullptr)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		if (m_pBodyColliderCom != nullptr)
			m_pBodyColliderCom->Add_ColGroup();
		if (m_pDetectionColliderCom != nullptr)
			m_pDetectionColliderCom->Add_ColGroup();
		if (m_pAttackRangeColliderCom != nullptr)
			m_pAttackRangeColliderCom->Add_ColGroup();
	}
	Safe_Release(pGameInstance);
}

HRESULT CPaladin::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		// m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CPaladin::OnCollisionEnter(const Collision* collision)
{

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(collision->OtherGameObject);
	_bool bPlayerBodyCollider = pPlayer && (collision->OtherCollider->GetName().compare(L"BodyCollider") == 0);
	_bool bPlayerAttackCollider = pPlayer && (collision->OtherCollider->GetName().compare(L"AttackCollider") == 0);

	if (bPlayerAttackCollider && collision->MyCollider == m_pBodyColliderCom)
	{
		m_isDamaged = true;
	}

	if (bPlayerBodyCollider)
	{
		if (collision->MyCollider == m_pDetectionColliderCom)
		{
			m_pBehaviorTreeCom->ChangeData("Target", collision->OtherGameObject);
		}

		if (collision->MyCollider == m_pAttackRangeColliderCom)
		{
			m_pBehaviorTreeCom->ChangeData("InAttackRange", true);
		}
	}

	
}

void CPaladin::OnCollisionStay(const Collision* collision)
{
}

void CPaladin::OnCollisionExit(const Collision* collision)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(collision->OtherGameObject);
	_bool bPlayerBodyCollider = pPlayer && (collision->OtherCollider->GetName().compare(L"BodyCollider") == 0);
	_bool bPlayerAttackCollider = pPlayer && (collision->OtherCollider->GetName().compare(L"AttackCollider") == 0);


	/*if (bPlayerAttackCollider && collision->MyCollider == m_pBodyColliderCom)
	{
		m_isDamaged = false;
	}*/

	/*if (dynamic_cast<CPlayer*>(collision->OtherGameObject) &&
		dynamic_cast<CPlayer*>(collision->OtherGameObject)->GetName().compare(L"BodyCollider") &&
		collision->MyCollider == m_pDetectionColliderCom)
	{
		cout << "EXITEXITEXITEXITEXITEXITEXITEXIT";
		m_pBehaviorTreeCom->ChangeData("Target", (CGameObject*)nullptr);
	}*/

	if (bPlayerBodyCollider && collision->MyCollider == m_pAttackRangeColliderCom)
	{
		m_pBehaviorTreeCom->ChangeData("InAttackRange", false);
	}
}

void CPaladin::Add_Animations()
{
	AnimNode node;

	////////////// 맨손 애니메이션들 //////////////
	node.bLoop = true;

	node.AnimIndices = { 67 };
	m_pAnimInstance->Push_Animation("Idle", node);

	node.AnimIndices = { 74 };
	m_pAnimInstance->Push_Animation("Walk", node);

	node.AnimIndices = { 68 };
	m_pAnimInstance->Push_Animation("Run", node);

	node.bLoop = false;

	node.AnimIndices = { 70, 71, 72 };
	node.eraseLessTime = { 9999, 999, 0.04 };
	m_pAnimInstance->Push_Animation("Smash", node);

	node.AnimIndices = { 60 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("Damaged", node);
}

void CPaladin::Attack()
{
}

void CPaladin::Damaged(const _float& TimeDelta)
{
	if (m_isDamaged == false ) return;

	cout << "Damaged" << endl;
	m_pBehaviorTreeCom->Stop();

	m_DamagedStunTimeAcc += TimeDelta;
	if (m_DamagedStunTimeAcc >= m_DamagedStunTime)
	{
		m_isDamaged = false;
		m_isSuperArmor = true;
		m_DamagedStunTimeAcc = 0.f;
	}
}

void CPaladin::SuperArmor(const _float& TimeDelta)
{
	if (m_isSuperArmor == false) return;
	
	m_pBehaviorTreeCom->Resume();

	m_SuperArmorTimeAcc += TimeDelta;
	if (m_SuperArmorTimeAcc >= m_SuperArmorTime)
	{
		m_isSuperArmor = false;
		m_SuperArmorTimeAcc = 0.f;
	}
}

void CPaladin::State()
{
	m_isRun = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsRun"));
	m_isWalk = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsWalk"));
	m_isAttack1 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack1"));
	// 랜덤 위치는 어디서 잡아주나?
}

void CPaladin::Select_DamagedKey()
{
	if (m_isDamaged)
	{
		m_pAnimInstance->Apply_Animation("Damaged");
	}
}

void CPaladin::Select_MoveKey()
{
	if (m_isRun == true)
	{
		m_pAnimInstance->Apply_Animation("Run");
		m_pTransformCom->Change_Speed(m_RunSpeed);
	}
	else if (m_isWalk == true)
	{
		m_pAnimInstance->Apply_Animation("Walk");
		m_pTransformCom->Change_Speed(m_WalkSpeed);
	}
		
}

void CPaladin::Select_IdleKey()
{
	m_pAnimInstance->Apply_Animation("Idle");
}

void CPaladin::Select_AttackKey()
{
	if (m_isAttack1)
	{
		m_pAnimInstance->Apply_Animation("Smash");
	}
}

void CPaladin::Select_AnimationKey()
{
	if (m_isDamaged && !m_isSuperArmor)
	{
		Select_DamagedKey();
		return;
	}


	if (m_isAttack1)
	{
		Select_AttackKey();
		return;
	}


	if (m_isWalk == true || m_isRun == true)
	{
		Select_MoveKey();
		return;
	}

	Select_IdleKey();

		
}

HRESULT CPaladin::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Paladin"),
		MODEL_W, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		NAVIGATION_W, (CComponent**)&m_pNavigationCom, &CNavigation::NAVIGATIONDESC(0))))
		return E_FAIL;

	CBounding_OBB::BOUNDINGBOX OBBDesc;

	OBBDesc.eColGroup = COL_ENEMY;
	OBBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	OBBDesc.vPosition = _float3(0.f, 0.f, 0.f);
	OBBDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		COLLIDER_W, (CComponent**)&m_pBodyColliderCom, &OBBDesc)))
		return E_FAIL;

	CBounding_Sphere::BOUNDINGSPHERE SphereDesc;

	SphereDesc.eColGroup = COL_DETECTION;
	SphereDesc.fRadius = 7.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		L"Sphere Detection", (CComponent**)&m_pDetectionColliderCom, &SphereDesc)))
		return E_FAIL;

	SphereDesc.eColGroup = COL_DETECTION;
	SphereDesc.fRadius = 3.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		L"SphereDistanceMaintain", (CComponent**)&m_pAttackRangeColliderCom, &SphereDesc)))
		return E_FAIL;

	////////////////////////// 애님 인스턴스 ///////////////
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimInstance"), L"AnimInstance", (CComponent**)&m_pAnimInstance, m_pModelCom)))
		return E_FAIL;


	
	////////////////////////// 비해비어 트리  ///////////////
	CBehaviorTree::BTDesc btDesc;
	btDesc.pRootNode = CRootNode::Create();
	btDesc.pBlackBoard = CBlackBoard::Create();

	btDesc.pBlackBoard->AddData("Owner", (CGameObject*)this);
	btDesc.pBlackBoard->AddData("Target", (CGameObject*)nullptr);
	btDesc.pBlackBoard->AddData("InAttackRange", false);

	btDesc.pBlackBoard->AddData("IsAttack1", false);
	btDesc.pBlackBoard->AddData("IsWalk", false);
	btDesc.pBlackBoard->AddData("IsRun", false);
	btDesc.pBlackBoard->AddData("RandomPos", _float3(0.f, 0.f, 0.f));


	

	// 블랙보드 정보를 추가하면 자동으로 데코레이터를 만들면 어떨까?
	btDesc.pRootNode->AddNode((CBehavior*)Test(3.0f,2.0f));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"), L"Behavior", (CComponent**)&m_pBehaviorTreeCom, &btDesc)))
		return E_FAIL;

	
	
	return S_OK;
}

HRESULT CPaladin::SetUp_ShaderResources()
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

CPaladin* CPaladin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPaladin* pInstance = new CPaladin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPaladin");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CPaladin::Clone(void* pArg)
{
	CPaladin* pInstance = new CPaladin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPaladin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPaladin::Free()
{
	__super::Free();
	Safe_Release(m_pAttackRangeColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pDetectionColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAnimInstance);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pBehaviorTreeCom);
	
	
}
