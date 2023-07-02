#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include "Calculator.h"
#include "Camera_Player_Main.h"
#include "PlayerHead.h"
#include "Mace.h"
#include "Axe.h"
#include "Slasher.h"
#include "PlayerParts.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreature(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CCreature(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	/* 서버로 부터 받아와야할 데이터를 셋팅하낟. */
	/* 파일로부터  받아와야할 데이터를 셋팅하낟. */
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	/* 원형생성할 때 받아왔던 데이터 외의 추가적인 값들을 더 저장해주낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Add_Animations();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_pModelCom->Remove_Mesh("Cape03Bot_GEO", 16);
	m_pModelCom->Remove_Mesh("Cape03Bot_GEO", 8);
	m_pModelCom->Collect_BodyBones_UpperLower(51, 17);
	Change_Weapon();

	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
 	__super::Tick(TimeDelta);

	if (m_pCameraCom->Is_On_Camera() == true)
	{
#ifdef _DEBUG
		if (CGameInstance::GetInstance()->Key_Down(DIK_C))
			CGameInstance::GetInstance()->Toggle_ColliderRender();
#endif
		CameraRotate(TimeDelta);
		PlayerRotate(TimeDelta);
		Select_Weapon();
		Skills(TimeDelta);
		Dash(TimeDelta);
		ClimbNavMesh();
		Move(TimeDelta);
		Attack_Combo();

		
		AttackMove(TimeDelta);
		CameraZoom(TimeDelta);
	}

	m_CurIndex = m_pNavigationCom->Get_CurrentIndex();


	// 애니메이션 관련된 변수 조작은 이 위에서
	Select_AnimationKey();


	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);	

	
	if (CGameInstance::GetInstance()->Key_Down(DIK_1))
		CGameInstance::GetInstance()->On_Camera(L"CameraFree");

	if (CGameInstance::GetInstance()->Key_Down(DIK_2))
		CGameInstance::GetInstance()->On_Camera(L"Camera_Player_Main");

	/*if (CGameInstance::GetInstance()->Key_Down(DIK_3))
		CGameInstance::GetInstance()->On_Shake();*/
}

HRESULT CPlayer::Render()
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



#ifdef _DEBUG
	m_pNavigationCom->Render_Navigation();
#endif

	

	return S_OK;
}

void CPlayer::OnCollisionEnter(const Collision* collision)
{
	
}

void CPlayer::OnCollisionStay(const Collision* collision)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 객체를 때렸을때 카메라 쉐이크를 넣기위함이다.
	if (collision->MyCollider == m_pColliderCom[COLLIDER_ATTACK])
	{
		m_ObjectAttack_CameraShake_TimeAcc += pGameInstance->Get_Timer(TEXT("Timer_60"));

		if (m_ObjectAttack_CameraShake_TimeAcc >= 0.5f)
		{
			if (pGameInstance->Is_Shacking() == true) return;

			CCamera_Player_Main::MAINCAMERASHAKE desc;

			m_ObjectAttack_CameraShake_TimeAcc = 0.0;
			desc.fShakeMagnitude = 0.08f;
			desc.fShakeDuration = 0.25f;
			pGameInstance->On_Shake(&desc);
		}

		
		
		
	}
}

void CPlayer::OnCollisionExit(const Collision* collision)
{
}

void CPlayer::Move(_double TimeDelta)
{
	if (m_bDash == true) return;
	if (m_bSkill == true) return;

	m_bMove = false;
	_bool keyState[4] = { false, false, false, false };
	_vector dir[4];

	if (CGameInstance::GetInstance()->Key_Pressing(DIK_W))
	{
		m_bMove = true;
		
		_vector offsetDir = m_pCameraCom->Get_Offset() * -1.f;
		dir[0].m128_f32[0] = offsetDir.m128_f32[0];
		dir[0].m128_f32[1] = 0.f;
		dir[0].m128_f32[2] = offsetDir.m128_f32[2];
		dir[0].m128_f32[3] = 0.f;

		keyState[0] = true;
	}
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_A))
	{
		m_bMove = true;

		_vector offsetDir = m_pCameraCom->Get_Offset();
		offsetDir = XMVector3Normalize(offsetDir);
		_vector upDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		dir[1] = XMVector3Cross(upDir, offsetDir);

		keyState[1] = true;
	}
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_S))
	{
		m_bMove = true;

		_vector offsetDir = m_pCameraCom->Get_Offset();
		dir[2].m128_f32[0] = offsetDir.m128_f32[0];
		dir[2].m128_f32[1] = 0.f;
		dir[2].m128_f32[2] = offsetDir.m128_f32[2];
		dir[2].m128_f32[3] = 0.f;

		keyState[2] = true;
	}
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_D))
	{
		m_bMove = true;

		_vector offsetDir = m_pCameraCom->Get_Offset();
		offsetDir = XMVector3Normalize(offsetDir);
		_vector upDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		dir[3] = -XMVector3Cross(upDir, offsetDir);

		keyState[3] = true;
	}

	if (m_bMove)
	{

		// 무기가 손 일때만 이동을 멈추게 하려고 했다. 추후 추가사항이 있으면 추가.
		if (m_bAttack == false || 
			m_eWeaponType != WEAPON_HAND)
		{
			_vector resultDir = { 0.f,0.f,0.f,0.f };


			for (int i = 0; i < 4; i++)
			{
				if (keyState[i] == true)
				{
					resultDir += XMVector3Normalize(dir[i]);
				}
			}

			XMStoreFloat3(&m_MoveDir, resultDir);

			m_pTransformCom->Go_Dir(resultDir, TimeDelta, m_pNavigationCom);
		}
		
	}

	// 지형 타는거 원래 최적화를 위해서 여기 뒀었다.
	// ClimbNavMesh();


	//if (CGameInstance::GetInstance()->Mouse_Pressing(CInput_Device::DIMK_LBUTTON))
	//{
	//	_float4 pos;
	//	if (CCalculator::Picking_Triangle(g_hWnd, g_iWinSizeX, g_iWinSizeY, m_pNavigationCom->Get_TriangleList(), &pos) >= 0)
	//	{
	//		_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//		pos.y = myPos.m128_f32[1];
	//		m_pTransformCom->LookAt(XMLoadFloat4(&pos));
	//		m_TargetPos = pos;
	//		m_bMove = true;
	//	}
	//}

	//if (m_bMove == true)
	//{
	//	_vector MovePosition = XMLoadFloat4(&m_TargetPos);
	//	if (m_pTransformCom->Chase(XMLoadFloat4(&m_TargetPos), TimeDelta, m_pNavigationCom, 0.15f) == false)
	//	{
	//		m_bMove = false;
	//	}

	//	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//	vector<Triangle>* vec = m_pNavigationCom->Get_TriangleList();
	//		
	//	// 평면의 방정식으로 점과 노말벡터를 넣으면 A B C D 를 뽑아줌
	//	_vector equationVec = XMPlaneFromPoints(XMLoadFloat3(&(*vec)[m_CurIndex].vVertex1), XMLoadFloat3(&(*vec)[m_CurIndex].vVertex2), XMLoadFloat3(&(*vec)[m_CurIndex].vVertex3));

	//	_float x = myPos.m128_f32[0];
	//	_float y = myPos.m128_f32[1];
	//	_float z = myPos.m128_f32[2];

	//	// 평면 방정식의 계수
	//	_float A = equationVec.m128_f32[0];
	//	_float B = equationVec.m128_f32[1];
	//	_float C = equationVec.m128_f32[2];
	//	_float D = equationVec.m128_f32[3];

	//	// 평면 위로의 이동량 계산
	//	_float offsetY = (-A * x - C * z - D) / B;		
	//
	//	myPos.m128_f32[1] = offsetY;

	//	m_pTransformCom->Set_Position(myPos);
	//}
}

void CPlayer::PlayerRotate(_double TimeDelta)
{
	if (m_bDash == true) return;


	POINT	ptMouse{};
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	POINT centerPos{};

	centerPos.x = g_iWinSizeX / 2;
	centerPos.y = g_iWinSizeY / 2;

	if (m_bRotate)
	{
		ptMouse = m_lastCursorPoint;
	}

	_double degree = atan2(ptMouse.y - centerPos.y, ptMouse.x - centerPos.x);
	degree = XMConvertToDegrees(degree);

	degree -= m_fCameraRotationX;



	m_pTransformCom->Rotation(CTransform::AXIS_Y, degree);
}

void CPlayer::AttackMove(_double TimeDelta)
{
	
	if (m_bAttackMove == false) return;

	

	m_MoveAttackTimeAcc += TimeDelta;

	if ((m_MoveAttackInitSpeed + (m_MoveAttackAccel * m_MoveAttackTimeAcc)) < 0.f)
	{
		m_bAttackMove = false;
		return;
	}

	_vector playerDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Go_Dir(playerDir, m_MoveAttackInitSpeed, m_MoveAttackAccel, m_MoveAttackTimeAcc, m_pNavigationCom);

	
}

void CPlayer::ClimbNavMesh()
{
	if (m_CurIndex == -1 || m_bClimbNavMesh == false) return;

	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	myPos.m128_f32[1] = Compute_NavMesh_Height();

	m_pTransformCom->Set_Position(myPos);
}

_float CPlayer::Compute_NavMesh_Height()
{
	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vector<Triangle>* vec = m_pNavigationCom->Get_TriangleList();

	// 평면의 방정식으로 점과 노말벡터를 넣으면 A B C D 를 뽑아줌
	_vector equationVec = XMPlaneFromPoints(XMLoadFloat3(&(*vec)[m_CurIndex].vVertex1), XMLoadFloat3(&(*vec)[m_CurIndex].vVertex2), XMLoadFloat3(&(*vec)[m_CurIndex].vVertex3));

	_float x = myPos.m128_f32[0];
	_float y = myPos.m128_f32[1];
	_float z = myPos.m128_f32[2];

	// 평면 방정식의 계수
	_float A = equationVec.m128_f32[0];
	_float B = equationVec.m128_f32[1];
	_float C = equationVec.m128_f32[2];
	_float D = equationVec.m128_f32[3];

	// 평면 위로의 이동량 계산
	_float offsetY = (-A * x - C * z - D) / B;

	return offsetY;
}

void CPlayer::CameraRotate(_double TimeDelta)
{
	if (CGameInstance::GetInstance()->Mouse_Up(CInput_Device::DIMK_RBUTTON))
	{
		m_bRotate = false;
		ShowCursor(true);
		ClientToScreen(g_hWnd, &m_lastCursorPoint);
		SetCursorPos(m_lastCursorPoint.x, m_lastCursorPoint.y);
		
	}
	if (CGameInstance::GetInstance()->Mouse_Pressing(CInput_Device::DIMK_RBUTTON))
	{
		

		POINT	ptMouse{}, ptCursor{};
		GetCursorPos(&ptMouse);	
		ScreenToClient(g_hWnd, &ptMouse);

		if (m_bRotate == false)
		{
			m_bRotate = true;
			ShowCursor(false);
			m_lastCursorPoint = ptMouse;
		}

		/*RECT clientRect;
		GetClientRect(g_hWnd, &clientRect);

		if (!PtInRect(&clientRect, ptMouse))
		{
			return;
		}*/

		


		_long		MouseMove = { 0 };

		if (MouseMove = CGameInstance::GetInstance()->Get_DIMouseMove(CInput_Device::DIMM_X))
		{
			
			m_fCameraRotationX += MouseMove * TimeDelta * 4.f;
			// x z 는 플레이어의 위치를 가져오고 y 값은 카메라의 위치를 가져온다.
			_vector myPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector offsetPos = m_pCameraCom->Get_Offset();


			_vector centerPos = XMVectorSet(myPos.m128_f32[0], myPos.m128_f32[1] + offsetPos.m128_f32[1], myPos.m128_f32[2], 1.f);

			_vector cameraPos = myPos + offsetPos;
			_vector dirVec = cameraPos - centerPos;

			_float length = XMVector3Length(dirVec).m128_f32[0];


			

			_float z = length * sinf(XMConvertToRadians(m_fCameraRotationX));
			_float x = length * cosf(XMConvertToRadians(m_fCameraRotationX));

			
			_vector offset = XMVectorSet(x, offsetPos.m128_f32[1], z, 1.f);


			m_pCameraCom->Set_Offset(offset);
		}

		if (MouseMove = CGameInstance::GetInstance()->Get_DIMouseMove(CInput_Device::DIMM_Y))
		{
			// 마우스가 중간에서 멀어질수록 알파 플러스가 있어야한다.

			// 30 ~ 60 도까지 허락 한다.
			_vector offsetPos = m_pCameraCom->Get_Offset();

			_float realAngle = -MouseMove * TimeDelta * 4.f;
			_float amount = m_fCameraRotationY - realAngle;

			if (m_fCameraRotationY == 40.f)
			{
				if (realAngle > 0) return;
			}
			else if (m_fCameraRotationY == 60.f)
			{
				if (realAngle < 0) return;
			}

			if (amount < 40.f)
			{
				m_fCameraRotationY = 40.f;
				realAngle = m_fCameraRotationY - amount;
			}
			else if (amount > 60.f)
			{
				m_fCameraRotationY = 60.f;
				realAngle = m_fCameraRotationY - amount;
			}
			else
			{
				m_fCameraRotationY = amount;
			}

			_vector offsetVec = XMVector3Normalize(offsetPos);
			_vector upVec = XMVectorSet(0.f, 1.f, 0.f, 0.f);

			_vector myAxis = XMVector3Cross(upVec, offsetVec);

			myAxis = XMVector3Normalize(myAxis);

			XMVECTOR quaternion = XMQuaternionRotationAxis(myAxis, XMConvertToRadians(realAngle));

			// 회전 적용하여 점 회전
			XMVECTOR rotatedPoint = XMVector3Rotate(offsetPos, quaternion);

			m_pCameraCom->Set_Offset(rotatedPoint);
		}

	}
}

void CPlayer::CameraZoom(_double TimeDelta)
{
	_long		MouseMove = { 0 };

	if (MouseMove = CGameInstance::GetInstance()->Get_DIMouseMove(CInput_Device::DIMM_WHEEL))
	{
		m_fCameraZoom += MouseMove * TimeDelta * 0.4f;

		if (m_fCameraZoom < 0.f)
		{
			m_fCameraZoom = 0.f;
		}
		else if (m_fCameraZoom > 10.f)
		{
			m_fCameraZoom = 10.f;
		}
	}

	// 오프셋을 뽑아와
	_vector offsetVec = m_pCameraCom->Get_Offset();

	_float originDis = XMVector3Length(XMLoadFloat3(&m_OriginOffset)).m128_f32[0];

	_vector dirVec = XMVector3Normalize(offsetVec);

		// 원래의 오프셋의 길이 originDis 에다가 카메라의 줌을 빼준다?
		// 카메라의 줌이 0이라면 원래의 오프셋길이일것이고 10이면 원래의 오프셋길이의 -10 만큼 가까워 질것이다.
	
	

	if (m_fCurCameraZoom < m_fCameraZoom)
	{
		m_fCurCameraZoom += 10.f * TimeDelta;
		if (m_fCurCameraZoom >= m_fCameraZoom)
		{
			m_fCurCameraZoom = m_fCameraZoom;
		}
			

	}
	else if (m_fCurCameraZoom > m_fCameraZoom)
	{
		m_fCurCameraZoom -= 10.f * TimeDelta;
		if (m_fCurCameraZoom < m_fCameraZoom)
		{
			m_fCurCameraZoom = m_fCameraZoom;
		}	
	}
	_vector zoomResult = dirVec * (originDis - m_fCurCameraZoom);

	m_pCameraCom->Set_Offset(zoomResult);

}

void CPlayer::Dash(_double TimeDelta)
{
	if (m_bDash == false)
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_SPACE))
		{
			m_bDash = true;

			m_pAnimInstance->Apply_Animation("Dash");
			
			XMStoreFloat3(&m_DashDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			m_DashTimeAcc = 0.0;
		}
	}
	

	if (m_bDash == true && m_pAnimInstance->Animation_Finished())
	{
		m_DashTimeAcc += TimeDelta;

		if (m_DashTimeAcc > 0.5f)
		{
			m_bDash = false;
			return;
		}

		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_DashDir), m_DashInitSpeed, m_DashAccel, m_DashTimeAcc / 4.0, m_pNavigationCom);
	}
}

void CPlayer::Select_ChangingWeaponKey()
{
	//if (m_eWeaponType == WEAPON_HAND) return;

	// 맨 손 무기같은 경우에는 교체 애니메이션이 없으므로 무시한다.
	if (m_pAnimInstance->Animation_Finished() || m_eWeaponType == WEAPON_HAND)
	{
		m_ePrevWeaponType = m_eWeaponType;
		return;
	}

	m_Weapons[m_eWeaponType].front()->Equip_Animation();
}

void CPlayer::Select_MoveKey()
{
	_vector lookVec = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	//// 룩벡터와 이동방향을 기반으로 계산하자
	//_float angle = XMConvertToDegrees(XMVector3AngleBetweenNormals(dir, lookVec).m128_f32[0]);
	_vector dir = XMLoadFloat3(&m_MoveDir);

	_float angle = XMConvertToDegrees(XMVector3AngleBetweenNormals(dir, lookVec).m128_f32[0]);
	XMVECTOR cross = XMVector3Cross(dir, lookVec);
	if (XMVectorGetX(XMVector3Dot(cross, {0.f,1.f,0.f,0.f})) > 0)
		angle = 360 - angle;



	
	if (337.5 <= angle || 22.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->N_Walk_Animation();
	}
	else if (22.5 <= angle && 67.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->NE_Walk_Animation();
	}
	else if (67.5 <= angle && 112.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->E_Walk_Animation();
	}
	else if (112.5 <= angle && 157.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->SE_Walk_Animation();
	}
	else if (157.5 <= angle && 202.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->S_Walk_Animation();
	}
	else if (202.5 <= angle && 247.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->SW_Walk_Animation();
	}
	else if (247.5 <= angle && 292.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->W_Walk_Animation();
	}
	else if (292.5 <= angle && 337.5 >= angle)
	{
		m_Weapons[m_eWeaponType].front()->NW_Walk_Animation();
	}

	
}

void CPlayer::Select_DashKey()
{
	
}

void CPlayer::Select_AttackKey()
{
	
}

void CPlayer::Select_IdleKey()
{
	m_Weapons[m_eWeaponType].front()->Idle_Animation();
}

void CPlayer::Attack_Combo()
{

	//cout << m_bAttack << " " << m_pAnimInstance->Is_Use_AllBodyAnim() << endl;

	// 공격이 끝나기 전 선입력을 하면... 콤보가 쌓이게끔.
	if (CGameInstance::GetInstance()->Mouse_Pressing(CInput_Device::DIMK_LBUTTON))
	{
		if (m_bAttack == true)
		{
			m_PrevCombo = m_Combo;
			m_Combo++;
		}
		else
		{
			if (m_bMove == false)
			{
				m_Weapons[m_eWeaponType].front()->Standing_Attack_Animation();
			}
			else
			{
				m_Weapons[m_eWeaponType].front()->Move_Attack_Animation();
			}

			m_bAttack = true;
		}

		/*if (m_pAnimInstance->Animation_Finished())
		m_ClawCombo */
	}
	
	if (m_bAttack == true)
	{
		
		m_pColliderCom[COLLIDER_ATTACK]->Set_Enable(true);

		// 지금은 All_Body 에 대해서 체크하고있다. 하지만 우리는 상체와 하체에 대해서도 검사해야한다.
		ANIMTYPE eType;

		if (m_pAnimInstance->Is_Use_AllBodyAnim())
			eType = ANIM_ALLBODY;
		else
			eType = ANIM_UPPERBODY;

		

		if (eType == ANIM_UPPERBODY && m_pAnimInstance->Get_UpperBody_CurAnimNode() == nullptr)
		{
			m_bAttack = false;
			return;
		}

		if (m_pAnimInstance->Animation_Finished(eType))
		{
			// 어택무브가 진행중이 아니였다라면

			if (m_bMove == true && m_pAnimInstance->Get_NextNode_Name(eType).compare("") != 0)
			{
				// 어택 무브가 들어가는 무기는 ? 현재 맨손밖에없음
				// 나중에 어택 무브가 들어가는 무기가 늘어나면 아래에 작성해주도록 하자.
				if (m_eWeaponType == WEAPON_HAND)
				{
					if (m_bAttackMove == false)
					{
						m_bAttackMove = true;
						m_MoveAttackTimeAcc = 0.0;
					}
					// 어택무브가 진행중 이였다면.
					else
					{
						m_MoveAttackTimeAcc = 0.0;
					}
				}
				
			}
			
			// 다음 Attack.
			if (m_Combo > m_PrevCombo)
			{
				_bool bNext = m_pAnimInstance->Next_Animation(eType);
				if (bNext == false)
				{
					m_pColliderCom[COLLIDER_ATTACK]->Set_Enable(false);
					m_bAttack = false;
					m_Combo = 0;
					m_PrevCombo = 0;
					return;
				}
				else
				{
					m_PrevCombo += 1;
				}
				
			}
			else
			{
				m_pColliderCom[COLLIDER_ATTACK]->Set_Enable(false);
				m_bAttack = false;
				m_Combo = 0;
				m_PrevCombo = 0;
			}
				
		}
	}
	
}

void CPlayer::Add_Animations()
{
	AnimNode node;

	////////////// 맨손 애니메이션들 //////////////
	node.bLoop = true;

	node.AnimIndices = { 0 };
	m_pAnimInstance->Push_Animation("N_Walk", node);
	node.AnimIndices = { 1 };
	m_pAnimInstance->Push_Animation("NE_Walk", node);
	node.AnimIndices = { 2 };
	m_pAnimInstance->Push_Animation("SE_Walk", node);
	node.AnimIndices = { 3 };
	m_pAnimInstance->Push_Animation("S_Walk", node);
	node.AnimIndices = { 4 };
	m_pAnimInstance->Push_Animation("SW_Walk", node);
	node.AnimIndices = { 5 };
	m_pAnimInstance->Push_Animation("NW_Walk", node);
	node.AnimIndices = { 6 };
	m_pAnimInstance->Push_Animation("W_Walk", node);
	node.AnimIndices = { 7 };
	m_pAnimInstance->Push_Animation("E_Walk", node);
	node.AnimIndices = { 8 };
	m_pAnimInstance->Push_Animation("Idle", node);

	node.bLoop = false;
	
	node.AnimIndices = {486, 487, 488 };
	node.eraseLessTime = { 0.05, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Claw_Right", node, "Claw_Left");

	node.AnimIndices = { 489, 490, 491 };
	m_pAnimInstance->Push_Animation("Claw_Right_Standing", node, "Claw_Left_Standing");

	node.AnimIndices = { 492, 493, 494 };
	node.eraseLessTime = { 0.05, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Claw_Left", node, "Claw_Smash");

	node.AnimIndices = { 495, 496, 497 };
	m_pAnimInstance->Push_Animation("Claw_Left_Standing", node, "Claw_Smash");

	node.AnimIndices = { 498, 499, 500 };
	node.eraseLessTime = { 0.06, 0.05, 0.3 };
	m_pAnimInstance->Push_Animation("Claw_Smash", node);

	node.AnimIndices = { 198, 199, 200 };
	node.eraseLessTime = { 0.06, 0.06, 0.03 };
	m_pAnimInstance->Push_Animation("Dash", node);

	////////////// 도끼 애니메이션들 //////////////

	node.bLoop = true;
	node.eraseLessTime = {};
	node.AnimIndices = { 84 };
	m_pAnimInstance->Push_Animation("Axe_N_Walk", node);
	node.AnimIndices = { 85 };
	m_pAnimInstance->Push_Animation("Axe_NE_Walk", node);
	node.AnimIndices = { 86 };
	m_pAnimInstance->Push_Animation("Axe_SE_Walk", node);
	node.AnimIndices = { 87 };
	m_pAnimInstance->Push_Animation("Axe_S_Walk", node);
	node.AnimIndices = { 88 };
	m_pAnimInstance->Push_Animation("Axe_SW_Walk", node);
	node.AnimIndices = { 89 };
	m_pAnimInstance->Push_Animation("Axe_NW_Walk", node);
	node.AnimIndices = { 90 };
	m_pAnimInstance->Push_Animation("Axe_W_Walk", node);
	node.AnimIndices = { 91 };
	m_pAnimInstance->Push_Animation("Axe_E_Walk", node);
	node.AnimIndices = { 92 };
	m_pAnimInstance->Push_Animation("Axe_Idle", node);

	node.bLoop = false;

	node.AnimIndices = { 138, 139, 143 };
	node.eraseLessTime = { 0.03, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Axe_Right", node, "Axe_Left");

	node.AnimIndices = { 140, 141, 142 };
	node.eraseLessTime = { 0.03, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Axe_Right_MoveCast", node, "Axe_Left_MoveCast");

	node.AnimIndices = { 144, 145, 149 };
	node.eraseLessTime = { 0.05, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Axe_Left", node, "Axe_Smash");

	node.AnimIndices = { 146, 147, 148 };
	node.eraseLessTime = { 0.05, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Axe_Left_MoveCast", node, "Axe_Smash_MoveCast");

	node.AnimIndices = { 150, 151, 155 };
	node.eraseLessTime = { 0.06, 0.05, 0.3 };
	m_pAnimInstance->Push_Animation("Axe_Smash", node);

	node.AnimIndices = { 152, 153, 154 };
	node.eraseLessTime = { 0.06, 0.05, 0.3 };
	m_pAnimInstance->Push_Animation("Axe_Smash_MoveCast", node);

	node.AnimIndices = { 157, 158 };
	node.eraseLessTime = {  0.01, 0.06 };
	m_pAnimInstance->Push_Animation("Axe_Dash", node);

	node.AnimIndices = { 159, 160, 161, 162 };
	node.eraseLessTime = { 0.0001, 0.0001, 0.00001, 0.4 };
	m_pAnimInstance->Push_Animation("Axe_Throw", node);

	node.AnimIndices = { 94 };
	node.eraseLessTime = { 0.1 };
	m_pAnimInstance->Push_Animation("Axe_Equip", node);

	//.//////////////////// 슬래셔 애니메이션 //////////////////////////

	node.bLoop = true;
	node.eraseLessTime = {};
	node.AnimIndices = { 72 };
	m_pAnimInstance->Push_Animation("Slasher_N_Walk", node);
	node.AnimIndices = { 73 };
	m_pAnimInstance->Push_Animation("Slasher_NE_Walk", node);
	node.AnimIndices = { 74 };
	m_pAnimInstance->Push_Animation("Slasher_SE_Walk", node);
	node.AnimIndices = { 75 };
	m_pAnimInstance->Push_Animation("Slasher_S_Walk", node);
	node.AnimIndices = { 76 };
	m_pAnimInstance->Push_Animation("Slasher_SW_Walk", node);
	node.AnimIndices = { 77 };
	m_pAnimInstance->Push_Animation("Slasher_NW_Walk", node);
	node.AnimIndices = { 78 };
	m_pAnimInstance->Push_Animation("Slasher_W_Walk", node);
	node.AnimIndices = { 79 };
	m_pAnimInstance->Push_Animation("Slasher_E_Walk", node);

	node.AnimIndices = { 80 };
	m_pAnimInstance->Push_Animation("Slasher_Idle", node);

	node.bLoop = false;

	node.AnimIndices = { 440, 441, 442 };
	node.eraseLessTime = { 0.03, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Slasher_Right", node, "Slasher_Left");

	node.AnimIndices = { 443, 444, 445};
	node.eraseLessTime = { 0.05, 0.05, 0.04 };
	m_pAnimInstance->Push_Animation("Slasher_Left", node, "Slasher_Smash");

	node.AnimIndices = { 446, 447, 448 };
	node.eraseLessTime = { 0.06, 0.05, 0.3 };
	m_pAnimInstance->Push_Animation("Slasher_Smash", node);

	node.AnimIndices = { 449, 450, 451 };
	node.eraseLessTime = { 0.001, 0.001, 0.001 };
	m_pAnimInstance->Push_Animation("Slasher_Q", node);

	node.AnimIndices = { 82 };
	node.eraseLessTime = { 0.02 };
	m_pAnimInstance->Push_Animation("Slasher_Equip", node);

	node.AnimIndices = { 82 };
	node.eraseLessTime = { 0.1 };
	m_pAnimInstance->Push_Animation("Slasher_Ready", node);

	/////////////////////// 메이스 애니메이션 //////////////////////////

	node.bLoop = true;
	node.eraseLessTime = {};
	node.AnimIndices = { 23 };
	m_pAnimInstance->Push_Animation("Mace_N_Walk", node);
	node.AnimIndices = { 24 };
	m_pAnimInstance->Push_Animation("Mace_NE_Walk", node);
	node.AnimIndices = { 25 };
	m_pAnimInstance->Push_Animation("Mace_SE_Walk", node);
	node.AnimIndices = { 26 };
	m_pAnimInstance->Push_Animation("Mace_S_Walk", node);
	node.AnimIndices = { 27 };
	m_pAnimInstance->Push_Animation("Mace_SW_Walk", node);
	node.AnimIndices = { 28 };
	m_pAnimInstance->Push_Animation("Mace_NW_Walk", node);
	node.AnimIndices = { 29 };
	m_pAnimInstance->Push_Animation("Mace_W_Walk", node);
	node.AnimIndices = { 30 };
	m_pAnimInstance->Push_Animation("Mace_E_Walk", node);
	node.AnimIndices = { 31 };
	m_pAnimInstance->Push_Animation("Mace_Idle", node);

	node.bLoop = false;

	node.AnimIndices = { 277, 278, 279 };
	node.eraseLessTime = { 0.02, 0.02, 0.02 };
	m_pAnimInstance->Push_Animation("Mace_Right", node, "Mace_Left");

	node.AnimIndices = { 280, 281, 282 };
	node.eraseLessTime = { 0.02, 0.001, 0.00001 };
	m_pAnimInstance->Push_Animation("Mace_Left", node, "Mace_Smash");

	node.AnimIndices = { 283, 284, 285 };
	node.eraseLessTime = { 0.05, 0.05, 0.35 };
	m_pAnimInstance->Push_Animation("Mace_Smash", node);

	node.AnimIndices = { 286, 287, 288 };
	node.eraseLessTime = { 0.6, 0.06, 0.3 };
	//node.bResetAfterFinish = false;
	m_pAnimInstance->Push_Animation("Mace_Dash", node);


	node.AnimIndices = { 290, 291};
	node.eraseLessTime = { 0.001, 0.5};
	m_pAnimInstance->Push_Animation("Mace_Spin", node);

	node.bMotionCancel = false;

	node.AnimIndices = { 33 };
	node.eraseLessTime = { 0.1 };
	m_pAnimInstance->Push_Animation("Mace_Equip", node);
}

void CPlayer::Skills(const _double& TimeDelta)
{
	if (m_eWeaponType == WEAPON_HAND) return;
	m_bSkill = m_bSkillQ || m_bSkillC || m_bSkillR || m_bSkillE || m_bSkillT;

	Skill_Q(TimeDelta);
	Skill_C(TimeDelta);
	Skill_R(TimeDelta);
	Skill_E(TimeDelta);
	Skill_T(TimeDelta);
}

void CPlayer::Skill_Q(const _double& TimeDelta)
{
	if (m_bSkillQ)
	{
		// 슬래셔 일때는 슬래셔 스킬이 끝나야 아래를 진행해야함.
		if (m_pAnimInstance->Animation_Finished())
		{
			_bool bResult = false;
			bResult = m_Weapons[m_eWeaponType].front()->Skill_Q_End();

			if (bResult == true)
			{
				m_bSkillQ = false;
				return;
			}
		}

		m_Weapons[m_eWeaponType].front()->Skill_Q(TimeDelta);
	}
	else
	{
		// Q를 눌렀을때 스킬 Q 를 쓰기위한 설정
		if (CGameInstance::GetInstance()->Key_Down(DIK_Q))
		{
			m_bSkillQ = true;
			m_Weapons[m_eWeaponType].front()->Skill_Q_Setting();
		}
	}
	
	
}

void CPlayer::Skill_C(const _double& TimeDelta)
{
	if (m_bSkillC)
	{
		if (m_pAnimInstance->Animation_Finished())
		{
			m_bSkillC = false;
			return;
		}

		switch (m_eWeaponType)
		{
		case WEAPON_AXE:
		{

		}
		break;
		default:
			break;
		}
	}
	else
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_C))
		{
			m_bSkillC = true;

			switch (m_eWeaponType)
			{
			default:
				break;
			}
		}
	}
	
}

void CPlayer::Skill_R(const _double& TimeDelta)
{
	if (CGameInstance::GetInstance()->Key_Down(DIK_R))
	{
		m_bSkillR = true;
	}
}

void CPlayer::Skill_E(const _double& TimeDelta)
{
	if (m_bSkillE)
	{
		if (m_pAnimInstance->Animation_Finished())
		{
			_bool bResult = false;
			bResult = m_Weapons[m_eWeaponType].front()->Skill_E_End();

			if (bResult == true)
			{
				m_bSkillE = false;
				return;
			}
		}

		m_Weapons[m_eWeaponType].front()->Skill_E(TimeDelta);
	}
	else
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_E))
		{
			m_bSkillE = true;
			m_Weapons[m_eWeaponType].front()->Skill_E_Setting();
		}
	}
}

void CPlayer::Skill_T(const _double& TimeDelta)
{
	if (CGameInstance::GetInstance()->Key_Down(DIK_T))
	{
		m_bSkillT = true;
	}
}

void CPlayer::Select_Weapon()
{
	if (CGameInstance::GetInstance()->Key_Down(DIK_X))
	{
		m_ePrevWeaponType = m_eWeaponType;
		m_eWeaponType = (WEAPONTYPE)(m_eWeaponType + 1) ;
		m_eWeaponType = m_eWeaponType >= WEAPON_END ? (WEAPONTYPE)0 : m_eWeaponType;
		Change_Weapon();
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_Z))
	{
		m_ePrevWeaponType = m_eWeaponType;
		m_eWeaponType = (WEAPONTYPE)(m_eWeaponType - 1);
		m_eWeaponType = m_eWeaponType <= -1 ? (WEAPONTYPE)(WEAPON_END-1): m_eWeaponType;
		Change_Weapon();
	}
}

void CPlayer::Change_Weapon()
{
	for (int i = 0; i < WEAPON_END; i++)
	{
		for (auto& weapon : m_Weapons[i])
		{
			if (weapon != nullptr)
			{
				if (i == m_eWeaponType)
				{
					weapon->Set_Enabled(true);
				}
				else
				{
					weapon->Set_Enabled(false);
				}

			}
		}
	}

	
}

void CPlayer::Select_AnimationKey()
{
	// 우선순위			1. 공격		2. 움직임		3. IDLE

	if (m_bSkillQ == true || m_bSkillC == true || 
		m_bSkillR == true || m_bSkillE == true || 
		m_bSkillT == true)
	{
		return;
	}

	if (m_ePrevWeaponType != m_eWeaponType)
	{
		Select_ChangingWeaponKey();
		return;
	}

	if (m_bDash == true)
	{
		Select_DashKey();
		return;
	}

	//if (m_bAttack == true)
	//{
	//	// 공격과 관련된 키 설정 함수
	//	Select_AttackKey();
	//	//return;
	//}
	if (m_bAttack == true && m_eWeaponType == WEAPON_HAND) return;

	if (m_bMove == true)
	{
		Select_MoveKey();
		return;
	}
	
	
	if (m_bAttack == true)
		return;

	Select_IdleKey();
	
	
}

HRESULT CPlayer::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		RENDERER_W, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	/*CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.SpeedPerSec = 10.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);*/

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TRANSFORM_W, (CComponent**)&m_pTransformCom, &CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f)))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		MODEL_W, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		SHADER_W, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Navigation */ 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		NAVIGATION_W, (CComponent**)&m_pNavigationCom, &CNavigation::NAVIGATIONDESC(0))))
		return E_FAIL;

	CCamera_Player_Main::CAMERAMAINPLAYERDESC		CameraMainPlayerDesc;

	CameraMainPlayerDesc.iData = { 0 };
	CameraMainPlayerDesc.CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraMainPlayerDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraMainPlayerDesc.CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraMainPlayerDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraMainPlayerDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraMainPlayerDesc.CameraDesc.fNear = 0.2f;
	CameraMainPlayerDesc.CameraDesc.fFar = 300.f;
	CameraMainPlayerDesc.CameraDesc.TransformDesc.SpeedPerSec = 9.f;
	CameraMainPlayerDesc.CameraDesc.TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(L"Prototype_GameObject_Camera_Player_Main", L"Camera_Player_Main", (CGameObject**)&m_pCameraCom, &CameraMainPlayerDesc)))
		return E_FAIL;

	CGameInstance::GetInstance()->On_Camera(m_pCameraCom);

	m_pCameraCom->Set_Offset(XMLoadFloat3(&m_OriginOffset));
	m_fCameraRotationX = 270.f;
	m_fCameraRotationY = 45.f;


	/* For.PlayerCollider */
	CBounding_OBB::BOUNDINGBOX OBBDesc;

	OBBDesc.eColGroup = COL_PLAYER;
	OBBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	OBBDesc.vPosition = _float3(0.f, OBBDesc.vExtents.y, 0.f);
	OBBDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		COLLIDER_W, (CComponent**)&m_pColliderCom[COLLIDER_PLAYER], &OBBDesc)))
		return E_FAIL;

	/* For.AttackCollider */
	CBounding_OBB::BOUNDINGBOX AttackColliderDesc;

	AttackColliderDesc.eColGroup = COL_PLAYERWEAPON;
	AttackColliderDesc.vExtents = _float3(0.3f, 0.3f, 0.3f);
	AttackColliderDesc.vPosition = _float3(0.f, 0.4f, 1.4f);
	AttackColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		L"Attack_Collider", (CComponent**)&m_pColliderCom[COLLIDER_ATTACK], &AttackColliderDesc)))
		return E_FAIL;

	m_pColliderCom[COLLIDER_ATTACK]->Set_Enable(false);
	

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/////////////////// 플레이어 머리///////////////////////////
	CPlayerHead::PARENTDESC		ParentDesc;
	ZeroMemory(&ParentDesc, sizeof ParentDesc);

	const CBone* pBone = m_pModelCom->Get_Bone("MagicaHeadColliderOffset");
	if (nullptr == pBone)
		return E_FAIL;

	ParentDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	ParentDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_PlayerHead"), L"PlayerHead", nullptr, &ParentDesc)))
		return E_FAIL;

	///////////// 손 이라는 무기는 없음 ~~~ ///////////

	m_Weapons[WEAPON_HAND].push_back(nullptr);
	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_Hand"), L"Hand", (CGameObject**)&m_Weapons[WEAPON_HAND][0], nullptr)))
		return E_FAIL;


	//////////////////////// 도끼1 ///////////////////////////////////
	CAxe::PARENTDESC		ParentDescAxe;
	ZeroMemory(&ParentDescAxe, sizeof ParentDescAxe);

	pBone = m_pModelCom->Get_Bone("Weapon01_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	ParentDescAxe.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentDescAxe.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentDescAxe.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	ParentDescAxe.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	m_Weapons[WEAPON_AXE].push_back(nullptr);
	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_Axe"), L"Axe", (CGameObject**)&m_Weapons[WEAPON_AXE][0], &ParentDescAxe)))
		return E_FAIL;

	//////////////////////// 도끼2 ///////////////////////////////////
	CAxe::PARENTDESC		ParentDescAxe2;
	ZeroMemory(&ParentDescAxe2, sizeof ParentDescAxe2);

	pBone = m_pModelCom->Get_Bone("Weapon02_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	ParentDescAxe2.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentDescAxe2.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentDescAxe2.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	ParentDescAxe2.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	m_Weapons[WEAPON_AXE].push_back(nullptr);
	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_Axe"), L"Axe2", (CGameObject**)&m_Weapons[WEAPON_AXE][1], &ParentDescAxe2)))
		return E_FAIL;

	////////////////////////// 슬래셔1 /////////////////////////
	CSlasher::PARENTDESC		ParentDescSlahser;
	ZeroMemory(&ParentDescSlahser, sizeof ParentDescSlahser);

	pBone = m_pModelCom->Get_Bone("Weapon01_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	ParentDescSlahser.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentDescSlahser.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentDescSlahser.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	ParentDescSlahser.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	m_Weapons[WEAPON_SLASHER].push_back(nullptr);
	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_Slasher"), L"Slasher", (CGameObject**)&m_Weapons[WEAPON_SLASHER][0], &ParentDescSlahser)))
		return E_FAIL;

	////////////////////////// 슬래셔2 /////////////////////////
	CSlasher::PARENTDESC		ParentDescSlahser2;
	ZeroMemory(&ParentDescSlahser2, sizeof ParentDescSlahser2);

	pBone = m_pModelCom->Get_Bone("Weapon02_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	ParentDescSlahser2.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentDescSlahser2.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentDescSlahser2.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	ParentDescSlahser2.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	m_Weapons[WEAPON_SLASHER].push_back(nullptr);
	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_Slasher"), L"Slasher2", (CGameObject**)&m_Weapons[WEAPON_SLASHER][1], &ParentDescSlahser2)))
		return E_FAIL;

	//////////////////////////// 메이스 ///////////////////////////
	CMace::PARENTDESC		ParentDescMace;
	ZeroMemory(&ParentDescMace, sizeof ParentDescMace);

	pBone = m_pModelCom->Get_Bone("Weapon02_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	ParentDescMace.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentDescMace.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentDescMace.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	ParentDescMace.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	m_Weapons[WEAPON_MACE].push_back(nullptr);
	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_Mace"), L"Mace", (CGameObject**)&m_Weapons[WEAPON_MACE][0], &ParentDescMace)))
		return E_FAIL;

	/////////////////////////////////////////////////////////////////////
	//////////////////////// 갑옷, 장비들 //////////////////////////////
	//////////////////////////////////////////////////////////////////


	//////////////////////////// 바지 ///////////////////////////////////
	CPlayerParts::PARENTDESC		PantsDesc;
	ZeroMemory(&PantsDesc, sizeof PantsDesc);

	pBone = m_pModelCom->Get_Bone("Pelvis_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	PantsDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	PantsDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	PantsDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	PantsDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	PantsDesc.ModelName = TEXT("Prototype_Component_Model_Pants_GEO #210964");
	_matrix localMatrix = XMMatrixIdentity();
	localMatrix = XMMatrixScaling(0.016f, 0.016f, 0.016f) * XMMatrixTranslation(0.f, -0.60f, 0.0f);
	XMStoreFloat4x4(&PantsDesc.LocalOffsetMatrix, localMatrix);

	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_PlayerParts"), L"PlayerPants", nullptr, &PantsDesc)))
		return E_FAIL;

	//////////////////////////// 갑옷 ///////////////////////////////////
	CPlayerParts::PARENTDESC		ArmorDesc;
	ZeroMemory(&ArmorDesc, sizeof ArmorDesc);

	pBone = m_pModelCom->Get_Bone("Ribcage_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	ArmorDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ArmorDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ArmorDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	ArmorDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	ArmorDesc.ModelName = TEXT("Prototype_Component_Model_Armor");
	localMatrix = XMMatrixIdentity();
	localMatrix = XMMatrixScaling(0.016f, 0.016f, 0.016f) * XMMatrixTranslation(0.f, 1.40f, 0.050f);
	XMStoreFloat4x4(&ArmorDesc.LocalOffsetMatrix, localMatrix);

	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_PlayerParts"), L"PlayerArmor", nullptr, &ArmorDesc)))
		return E_FAIL;

	//////////////////////////// 헬멧 ///////////////////////////////////
	CPlayerParts::PARENTDESC		HelmetDesc;
	ZeroMemory(&HelmetDesc, sizeof HelmetDesc);

	pBone = m_pModelCom->Get_Bone("Hat_JNT");
	if (nullptr == pBone)
		return E_FAIL;

	HelmetDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	HelmetDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	HelmetDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix();
	HelmetDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	HelmetDesc.ModelName = TEXT("Prototype_Component_Model_Bear_GEO2");
	localMatrix = XMMatrixIdentity();
	localMatrix = XMMatrixScaling(0.009f, 0.009f, 0.009f) * XMMatrixRotationAxis({0.f,1.f,0.f,0.f}, XMConvertToRadians(270.f)) * XMMatrixRotationAxis({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(-90.f)) * XMMatrixTranslation(-0.18f, 0.05f,0.f);
	XMStoreFloat4x4(&HelmetDesc.LocalOffsetMatrix, localMatrix);

	if (FAILED(__super::Add_Component(TEXT("Prototype_GameObject_PlayerParts"), L"PlayerHelmet", nullptr, &HelmetDesc)))
		return E_FAIL;


	////////////////////////// 애님 인스턴스 ///////////////
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimInstance"), L"AnimInstance", (CComponent**)&m_pAnimInstance, m_pModelCom)))
		return E_FAIL;


	for (_int i = 0; i < WEAPON_END; i++)
	{
		for (auto& weapon : m_Weapons[i])
		{
			weapon->SetPlayer();
		}
	}

	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
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

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	/*for (auto& pCollider : m_pColliderCom)
		Safe_Release(pCollider);*/

	for (_int i = 0; i < WEAPON_END; i++)
	{
		for (auto& weapon : m_Weapons[i])
		{
			Safe_Release(weapon);
		}
	}

	Safe_Release(m_pAnimInstance);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCameraCom);
	Safe_Release(m_pColliderCom[COLLIDER_PLAYER]);
	Safe_Release(m_pColliderCom[COLLIDER_ATTACK]);
}
