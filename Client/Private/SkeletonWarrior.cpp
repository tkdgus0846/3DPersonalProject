#include "stdafx.h"
#include "..\Public\SkeletonWarrior.h"
#include "GameInstance.h"
#include "RootNode.h"
#include "BlackBoard.h"
#include <Player.h>
#include "Camera_Player_Main.h"
#include "Calculator.h"

CSkeletonWarrior::CSkeletonWarrior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CSkeletonWarrior::CSkeletonWarrior(const CSkeletonWarrior& rhs)
	: CMonster(rhs)
{
}

HRESULT CSkeletonWarrior::Initialize_Prototype()
{
	/* 서버로 부터 받아와야할 데이터를 셋팅하낟. */
	/* 파일로부터  받아와야할 데이터를 셋팅하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkeletonWarrior::Initialize(void* pArg)
{
	/* 원형생성할 때 받아왔던 데이터 외의 추가적인 값들을 더 저장해주낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_pAnimInstance->Apply_Animation("Idle");

	//m_pTransformCom->Set_Position({ 1.f,0.f,10.f,1.f });
	m_pTransformCom->Change_Speed(m_WalkSpeed);

	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	m_DamagedStunTime = 1.0f;
	m_SuperArmorTime = 2.5f;
	m_RunSpeed = 3.5f;

	Set_HP(50);

	return S_OK;
}

HRESULT CSkeletonWarrior::Ready_BehaviorTree()
{
	////////////////////////// 비해비어 트리  ///////////////
	CBehaviorTree::BTDesc btDesc(this);

	// 블랙보드 정보를 추가하면 자동으로 데코레이터를 만들면 어떨까?
	btDesc.pRootNode->AddNode((CBehavior*)SkeletonAI(1));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"), L"Behavior", (CComponent**)&m_pBehaviorTreeCom, &btDesc)))
		return E_FAIL;
	
	return S_OK;
}

void CSkeletonWarrior::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	Appear(TimeDelta);
	/*m_pBehaviorTreeCom->ChangeData("TargetDistance", Compute_TargetDistance());*/
	ClimbNavMesh();
	Navigation_CurIndex();
}

_int CSkeletonWarrior::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pTarget = any_cast<CGameObject*>(m_pBehaviorTreeCom->GetData("Target"));
	if (pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (pTarget != nullptr)
		{
			if (m_pRendererCom != nullptr)
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		}
		if (m_pBodyColliderCom != nullptr)
			m_pBodyColliderCom->Add_ColGroup();
		if (m_pDetectionColliderCom != nullptr)
			m_pDetectionColliderCom->Add_ColGroup();
		if (m_pAttackRangeColliderCom != nullptr)
			m_pAttackRangeColliderCom->Add_ColGroup();
	}
	Safe_Release(pGameInstance);

	if (GetDead())
		return OBJ_DEAD;
	
	return OBJ_NOEVENT;
}

HRESULT CSkeletonWarrior::Render()
{
	if (FAILED(__super::Render()))
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

void CSkeletonWarrior::OnCollisionEnter(const Collision* collision)
{

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(collision->OtherGameObject);
	_bool bPlayerBodyCollider = pPlayer && (collision->OtherCollider->GetName().compare(L"BodyCollider") == 0);

	if (bPlayerBodyCollider)
	{
		if (collision->MyCollider == m_pDetectionColliderCom)
		{
			m_pBehaviorTreeCom->ChangeData("Target", collision->OtherGameObject);
			m_pBehaviorTreeCom->ChangeData("InDetectionRange", true);

			if (m_bAppeard == false)
			{
				LookTarget();
				m_pAnimInstance->Apply_Animation("Appear");
				m_bAppeard = true;
			}
		}

		if (collision->MyCollider == m_pAttackRangeColliderCom)
		{
			m_pBehaviorTreeCom->ChangeData("InAttackRange", true);
		}
	}

	
}

void CSkeletonWarrior::OnCollisionStay(const Collision* collision)
{	
	__super::OnCollisionStay(collision);
}

void CSkeletonWarrior::OnCollisionExit(const Collision* collision)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(collision->OtherGameObject);
	_bool bPlayerBodyCollider = pPlayer && (collision->OtherCollider->GetName().compare(L"BodyCollider") == 0);

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

	// 디텍션 콜라이더에서 나갔을때. 
	if (bPlayerBodyCollider)
	{
		if (collision->MyCollider == m_pDetectionColliderCom)
		{
			m_pBehaviorTreeCom->ChangeData("InDetectionRange", false);
		}
	}
}

HRESULT CSkeletonWarrior::Add_Animations()
{
	AnimNode node;

	////////////// 맨손 애니메이션들 //////////////
	node.bLoop = true;

	node.AnimIndices = { 27 };
	m_pAnimInstance->Push_Animation("Idle", node);

	/*node.AnimIndices = { 72 };
	m_pAnimInstance->Push_Animation("Walk", node);*/

	node.AnimIndices = { 11 };
	m_pAnimInstance->Push_Animation("Run", node);

	node.bLoop = false;

	node.AnimIndices = { 0 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("Damaged", node);

	node.AnimIndices = { 19 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("Appear", node);

	node.AnimIndices = { 23, 25 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("AtackFullCombo", node);

	node.AnimIndices = { 15 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("Death", node);

	return S_OK;
}

void CSkeletonWarrior::Attack(const _float& TimeDelta)
{
	if (m_IsAttack1)
	{
		ComboAttack(TimeDelta);
		return;
	}
		
}

void CSkeletonWarrior::Damaged(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Damaged");

	cout << "Damaged" << endl;
	m_pBehaviorTreeCom->Stop();

	m_DamagedStunTimeAcc += TimeDelta;
	if (m_DamagedStunTimeAcc >= m_DamagedStunTime)
	{
		m_pBehaviorTreeCom->Resume();

		m_isDamaged = false;
		m_isSuperArmor = true;
		m_DamagedStunTimeAcc = 0.f;
	}
}

// 슈퍼 아머때 맞으면 지가 하던 애니메이션 실행하다가 데미지 받은 애니메이션을 실행한다.
void CSkeletonWarrior::SuperArmor(const _float& TimeDelta)
{	
	m_SuperArmorTimeAcc += TimeDelta;
	if (m_SuperArmorTimeAcc >= m_SuperArmorTime)
	{
		m_isSuperArmor = false;
		m_SuperArmorTimeAcc = 0.f;
	}

	m_isDamaged = false;
}

void CSkeletonWarrior::Walk(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Walk");
	m_pTransformCom->Change_Speed(m_WalkSpeed);
}

void CSkeletonWarrior::Run(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Run");
	m_pTransformCom->Change_Speed(m_RunSpeed);
}

void CSkeletonWarrior::KnockBack(const _float& TimeDelta)
{
}

void CSkeletonWarrior::Stun(const _float& TimeDelta)
{
}

void CSkeletonWarrior::Idle(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Idle");
}

void CSkeletonWarrior::Death(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Death");
	m_pBehaviorTreeCom->Stop();

	m_pBodyColliderCom->Set_Enable(false);
	m_pDetectionColliderCom->Set_Enable(false);
	m_pAttackRangeColliderCom->Set_Enable(false);

	m_DeathTimeAcc += TimeDelta;

	if (m_DeathTimeAcc >= m_DeathTime)
	{
		m_isDeathFinished = true;
	}
}

void CSkeletonWarrior::Appear(const _float& TimeDelta)
{
	if (m_bAppeard == true && m_bAppeardFinished == false)
	{
		m_pBehaviorTreeCom->Stop();
		if (m_pAnimInstance->Animation_Finished())
		{
			m_pBehaviorTreeCom->Resume();
			m_bAppeardFinished = true;
		}
	}
}

void CSkeletonWarrior::State()
{
	if (m_bAppeardFinished == false) return;

	m_isRun = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsRun"));
	m_isWalk = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsWalk"));
	m_IsAttack1 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack1"));
	m_IsAttack2 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack2"));
	m_IsAttack3 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack3"));
	m_IsAttack4 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack4"));

	_int iPriority = 0;

	if (m_bAirborne == true)
	{
		Add_State(STATE_AIRBORNE, iPriority++);
		return;
	}

	if (m_isDeathReserve == true)
	{
		Add_State(STATE_DEATH, iPriority++);
		return;
	}

	if (m_isDamaged && !m_isSuperArmor)
	{
		Add_State(STATE_DAMAGED, iPriority++);
		return;
	}
	else if (m_isSuperArmor)
	{
		Add_State(STATE_SUPERARMOR, iPriority++);
	}


	if (m_IsAttack1 || m_IsAttack2 || m_IsAttack3 || m_IsAttack4)
	{
		m_pBehaviorTreeCom->ChangeData("IsAttacking", true);
		Add_State(STATE_ATTACK, iPriority++);
		return;
	}
	else
	{
		m_pBehaviorTreeCom->ChangeData("IsAttack1Finished", false);
		m_pBehaviorTreeCom->ChangeData("IsAttack2Finished", false);
		m_pBehaviorTreeCom->ChangeData("IsAttack3Finished", false);
		m_pBehaviorTreeCom->ChangeData("IsAttack4Finished", false);

		m_IsAttack1Started = true;
		m_IsAttack2Started = true;
		m_IsAttack3Started = true;
		m_IsAttack4Started = true;
	}
	
	
	if (m_isWalk == true || m_isRun == true)
	{
		if (m_isRun == true)
			Add_State(STATE_RUN, iPriority++);
		else if (m_isWalk == true && any_cast<_bool>(m_pBehaviorTreeCom->GetData("InAttackRange")) == false)
			Add_State(STATE_WALK, iPriority++);

		return;
	}
	
	Add_State(STATE_IDLE, iPriority++);
}

void CSkeletonWarrior::ComboAttack(const _float& TimeDelta)
{
	if (m_pAnimInstance->Animation_Finished())
	{
		_bool bNext = m_pAnimInstance->Next_Animation();
		if (bNext == false)
		{
			//cout << "IsAttack2Finished" << endl;
			m_pBehaviorTreeCom->ChangeData("IsAttack1Finished", true);
			m_pBehaviorTreeCom->ChangeData("IsAttacking", false);
			m_IsAttack1Started = true;
			return;
		}
	}

	if (m_IsAttack1Started)
	{
		LookTarget();
		m_pAnimInstance->Apply_Animation("AtackFullCombo");
		m_IsAttack1Started = false;
	}
}

HRESULT CSkeletonWarrior::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SkeletonWarrior"),
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
	OBBDesc.vExtents = _float3(0.5f, 1.0f, 0.5f);
	OBBDesc.vPosition = _float3(0.f, 1.0f, 0.f);
	OBBDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		L"BodyCollider", (CComponent**)&m_pBodyColliderCom, &OBBDesc)))
		return E_FAIL;

	CBounding_Sphere::BOUNDINGSPHERE SphereDesc;

	SphereDesc.eColGroup = COL_DETECTION;
	SphereDesc.fRadius = 8.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		L"Sphere Detection", (CComponent**)&m_pDetectionColliderCom, &SphereDesc)))
		return E_FAIL;

	SphereDesc.eColGroup = COL_DETECTION;
	SphereDesc.fRadius = 2.f;
	SphereDesc.vPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		L"SphereDistanceMaintain", (CComponent**)&m_pAttackRangeColliderCom, &SphereDesc)))
		return E_FAIL;

	////////////////////////// 애님 인스턴스 ///////////////
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimInstance"), L"AnimInstance", (CComponent**)&m_pAnimInstance, m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkeletonWarrior::SetUp_ShaderResources()
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

CSkeletonWarrior* CSkeletonWarrior::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkeletonWarrior* pInstance = new CSkeletonWarrior(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSkeletonWarrior");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CSkeletonWarrior::Clone(void* pArg)
{
	CSkeletonWarrior* pInstance = new CSkeletonWarrior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSkeletonWarrior");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkeletonWarrior::Free()
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
