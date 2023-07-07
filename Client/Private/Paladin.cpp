#include "stdafx.h"
#include "..\Public\Paladin.h"
#include "GameInstance.h"
#include "RootNode.h"
#include "BlackBoard.h"
#include <Player.h>
#include "Camera_Player_Main.h"
#include "Calculator.h"

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

	m_pAnimInstance->Apply_Animation("Idle");

	m_pTransformCom->Set_Position({ 10.f,0.f,5.f,1.f });
	m_pTransformCom->Change_Speed(m_WalkSpeed);

	m_pBehaviorTreeCom->ChangeData("StandardDistance", 12.f);

	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	Set_HP(100);

	return S_OK;
}

HRESULT CPaladin::Ready_BehaviorTree()
{
	////////////////////////// 비해비어 트리  ///////////////
	CBehaviorTree::BTDesc btDesc(this);

	// 블랙보드 정보를 추가하면 자동으로 데코레이터를 만들면 어떨까?
	btDesc.pRootNode->AddNode((CBehavior*)PaladinAI(3.0f, 2.0f, 3));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"), L"Behavior", (CComponent**)&m_pBehaviorTreeCom, &btDesc)))
		return E_FAIL;

	return S_OK;
}

void CPaladin::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pBehaviorTreeCom->ChangeData("TargetDistance", Compute_TargetDistance());
	ClimbNavMesh();
	Navigation_CurIndex();
}

_int CPaladin::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (GetDead())
		int i = 2;

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

	if (GetDead())
		return OBJ_DEAD;
	
	return OBJ_NOEVENT;
}

HRESULT CPaladin::Render()
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
			m_pBehaviorTreeCom->ChangeData("InDetectionRange", true);
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

	// 디텍션 콜라이더에서 나갔을때. 
	if (bPlayerBodyCollider)
	{
		if (collision->MyCollider == m_pDetectionColliderCom)
		{
			m_pBehaviorTreeCom->ChangeData("InDetectionRange", false);
		}
	}
}

HRESULT CPaladin::Add_Animations()
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

	node.AnimIndices = { 4, 5 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("LeftSwing", node, "RightSwing");

	node.AnimIndices = { 8, 9, 10 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("RightSwing", node);

	node.AnimIndices = { 11, 12, 13 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("LeftFullSwing", node);

	node.AnimIndices = { 14, 15, 16 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("FullHammerDown", node);

	node.AnimIndices = { 17, 18 };
	node.eraseLessTime = { 9999, 999 };
	m_pAnimInstance->Push_Animation("Dash", node);

	node.AnimIndices = { 19, 20, 21, 22, 23 };
	node.eraseLessTime = { 9999, 999, 9999, 999, 9999 };
	m_pAnimInstance->Push_Animation("FloorSkill", node);

	node.AnimIndices = { 24, 26, 27 };
	node.eraseLessTime = { 9999, 999, 9999 };
	m_pAnimInstance->Push_Animation("HammerDownAfterLaunch", node);

	node.AnimIndices = { 28, 29, 30 };
	node.eraseLessTime = { 9999, 999, 9999 };
	m_pAnimInstance->Push_Animation("Joke", node);

	node.AnimIndices = { 36, 37, 38 };
	node.eraseLessTime = { 9999, 999, 9999 };
	m_pAnimInstance->Push_Animation("ThunderSkill", node);

	node.AnimIndices = { 60 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("Damaged", node);

	node.AnimIndices = { 40 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("TargetLock", node);

	node.AnimIndices = { 55 };
	node.eraseLessTime = {  };
	m_pAnimInstance->Push_Animation("Death", node);

	return S_OK;
}

void CPaladin::Attack(const _float& TimeDelta)
{
	
	if (m_IsAttack4)
	{
		cout << "대쉬 인 !!!!!!!!!!!!!!!" << endl;
		DashAttack(TimeDelta);
		return;
	}

	if (m_IsAttack1)
	{
		HammerDownAttack(TimeDelta);
		return;
	}
	
	if (m_IsAttack2)
	{
		SwingAttack(TimeDelta);
		return;
	}

	if (m_IsAttack3)
	{
		ThunderAttack(TimeDelta);
		return;
	}
		
}

void CPaladin::Damaged(const _float& TimeDelta)
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
void CPaladin::SuperArmor(const _float& TimeDelta)
{	
	m_SuperArmorTimeAcc += TimeDelta;
	if (m_SuperArmorTimeAcc >= m_SuperArmorTime)
	{
		m_isSuperArmor = false;
		m_SuperArmorTimeAcc = 0.f;
	}

	m_isDamaged = false;
}

void CPaladin::Walk(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Walk");
	m_pTransformCom->Change_Speed(m_WalkSpeed);
}

void CPaladin::Run(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Run");
	m_pTransformCom->Change_Speed(m_RunSpeed);
}

void CPaladin::KnockBack(const _float& TimeDelta)
{
}

void CPaladin::Stun(const _float& TimeDelta)
{
}

void CPaladin::Idle(const _float& TimeDelta)
{
	m_pAnimInstance->Apply_Animation("Idle");
}

void CPaladin::Death(const _float& TimeDelta)
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

void CPaladin::State()
{
	m_isRun = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsRun"));
	m_isWalk = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsWalk"));
	m_IsAttack1 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack1"));
	m_IsAttack2 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack2"));
	m_IsAttack3 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack3"));
	m_IsAttack4 = any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttack4"));
	/*if (m_isRun == true) cout << " m_isRun !!" << endl;

	if (m_IsAttack1 == true) cout << " m_IsAttack1 !!" << endl;
	if (m_IsAttack2 == true) cout << " m_IsAttack2 !!" << endl;
	if (m_IsAttack3 == true) cout << " m_IsAttack3 !!" << endl;*/

	/*cout << any_cast<_bool>(m_pBehaviorTreeCom->GetData("IsAttacking")) << endl;*/

	_int iPriority = 0;

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

void CPaladin::DashAttack(const _float& TimeDelta)
{
	if (m_pAnimInstance->Animation_Finished() && m_bMaceDashFinished == true)
	{
		m_pBehaviorTreeCom->ChangeData("IsAttack4Finished", true);
		m_pBehaviorTreeCom->ChangeData("IsAttacking", false);
		m_pBehaviorTreeCom->ChangeData("IsAttack4Started", true);
		// 애니메이션이 끝났을때 IsAttacking을 false 로 만든다. IsAttacking이 false 일때 해당 공격동작 말고 다른 태스크를 이용하게 하기 위함이다.
		m_IsAttack4Started = true;
		return;
	}

	if (m_IsAttack4Started)
	{
		LookTarget();
		m_pAnimInstance->Apply_Animation("HammerDownAfterLaunch");
		m_IsAttack4Started = false;
		m_pBehaviorTreeCom->ChangeData("IsAttack4Started", false);
		m_bMaceDashFinished = false;

		m_MaceDashJumpOriginHeight = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
		m_MaceDashTimeAcc = 0.0f;
		XMStoreFloat3(&m_MaceDashTargetPos, Compute_TargetPos());
		XMStoreFloat3(&m_MaceDashDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	}
	else
	{
		if (m_bMaceDashFinished == true) return;

		m_MaceDashTimeAcc += TimeDelta;

		// x z 로만 거리를 재야한다.
		
		_float distanceDashPos = CCalculator::Distance_Vector_XZ((XMLoadFloat3(&m_MaceDashTargetPos)), m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (distanceDashPos > 0.5f && ((m_MaceDashInitSpeed + (m_MaceDashAccel * m_MaceDashTimeAcc)) >= 0.f))
		{
			m_pTransformCom->Go_Dir(XMLoadFloat3(&m_MaceDashDir), m_MaceDashInitSpeed, m_MaceDashAccel, m_MaceDashTimeAcc, m_pNavigationCom);
		}

		_float height = m_MaceDashJumpOriginHeight + (m_MaceDashJumpSpeed * m_MaceDashTimeAcc) - (0.5 * m_MaceDashJumpGravity * m_MaceDashTimeAcc * m_MaceDashTimeAcc);

		if (Compute_NavMesh_Height() <= height)
		{
			_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			myPos.m128_f32[1] = height;
			m_pTransformCom->Set_Position(myPos);

			m_bClimbNavMesh = false;
		}
		else
		{
			CCamera_Player_Main::MAINCAMERASHAKE desc;
			desc.fShakeMagnitude = 0.2f;
			CGameInstance::GetInstance()->On_Shake(&desc);
			m_bClimbNavMesh = true;
			m_bMaceDashFinished = true;
		}
	}
}

void CPaladin::SwingAttack(const _float& TimeDelta)
{
	if (m_pAnimInstance->Animation_Finished())
	{
		_bool bNext = m_pAnimInstance->Next_Animation();
		if (bNext == false)
		{
			//cout << "IsAttack2Finished" << endl;
			m_pBehaviorTreeCom->ChangeData("IsAttack2Finished", true);
			m_pBehaviorTreeCom->ChangeData("IsAttacking", false);
			m_IsAttack2Started = true;
			return;
		}
	}

	if (m_IsAttack2Started)
	{
		//cout << "IsAttack2Start" << endl;
		LookTarget();
		m_pAnimInstance->Apply_Animation("LeftSwing");
		m_IsAttack2Started = false;
	}
}

void CPaladin::HammerDownAttack(const _float& TimeDelta)
{
	if (m_pAnimInstance->Animation_Finished())
	{
		m_pBehaviorTreeCom->ChangeData("IsAttack1Finished", true);
		m_pBehaviorTreeCom->ChangeData("IsAttacking", false);
		// 애니메이션이 끝났을때 IsAttacking을 false 로 만든다. IsAttacking이 false 일때 해당 공격동작 말고 다른 태스크를 이용하게 하기 위함이다.
		m_IsAttack1Started = true;
		return;
	}

	if (m_IsAttack1Started)
	{
		LookTarget();
		m_pAnimInstance->Apply_Animation("Smash");
		m_IsAttack1Started = false;
	}

	//cout << "IsAttack1Start" << endl;
}

void CPaladin::ThunderAttack(const _float& TimeDelta)
{
	if (m_pAnimInstance->Animation_Finished())
	{
		_bool bNext = m_pAnimInstance->Next_Animation();
		if (bNext == false)
		{
			//cout << "IsAttack3Finished" << endl;
			m_pBehaviorTreeCom->ChangeData("IsAttack3Finished", true);
			m_pBehaviorTreeCom->ChangeData("IsAttacking", false);
			m_IsAttack3Started = true;
			return;
		}
	}

	if (m_IsAttack3Started)
	{
		LookTarget();
		m_pAnimInstance->Apply_Animation("ThunderSkill");
		m_IsAttack3Started = false;
	}
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
	SphereDesc.fRadius = 8.f;
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
