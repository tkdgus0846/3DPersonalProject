#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"

#include "Calculator.h"
#include "Camera_Player_Main.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pModelCom->Set_AnimIndex(3);

	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
 	__super::Tick(TimeDelta);

	CameraRotate(TimeDelta);
	PlayerRotate(TimeDelta);
	Move(TimeDelta);
	CameraZoom(TimeDelta);

	m_CurIndex = m_pNavigationCom->Get_CurrentIndex();

	if (m_bMove == false)
		m_pModelCom->Set_AnimIndex(61);
	else
		m_pModelCom->Set_AnimIndex(74);


	m_pModelCom->Play_Animation(TimeDelta);

	/*for (auto& pCollider : m_pColliderCom)
	{
		if (nullptr != pCollider)
			pCollider->Tick(m_pTransformCom->Get_WorldMatrix());
	}*/
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
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

	/*for (auto& pCollider : m_pColliderCom)
	{
		if(nullptr != pCollider)
			pCollider->Render();
	}*/
	

#endif

	

	return S_OK;
}

void CPlayer::Move(_double TimeDelta)
{
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
		_vector resultDir = { 0.f,0.f,0.f,0.f };
		for (int i = 0; i < 4; i++)
		{
			if (keyState[i] == true)
			{
				resultDir += XMVector3Normalize(dir[i]);
			}
		}

		m_pTransformCom->Go_Dir(resultDir, TimeDelta, m_pNavigationCom);

		ClimbNavMesh();
	}


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

	//cout << degree << endl;

	m_pTransformCom->Rotation(CTransform::AXIS_Y, degree);
}

void CPlayer::ClimbNavMesh()
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

	myPos.m128_f32[1] = offsetY;

	m_pTransformCom->Set_Position(myPos);
}

void CPlayer::CameraRotate(_double TimeDelta)
{
	if (CGameInstance::GetInstance()->Mouse_Up(CInput_Device::DIMK_RBUTTON))
	{
		m_bRotate = false;
		ShowCursor(true);
		SetCursorPos(m_lastCursorPoint.x, m_lastCursorPoint.y);
		
	}
	if (CGameInstance::GetInstance()->Mouse_Pressing(CInput_Device::DIMK_RBUTTON))
	{
		

		POINT	ptMouse{}, ptCursor{};
		GetCursorPos(&ptMouse);	

		if (m_bRotate == false)
		{
			m_bRotate = true;
			ShowCursor(false);
			m_lastCursorPoint = ptMouse;
		}

		ScreenToClient(g_hWnd, &ptMouse);

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
			return;
		}
		else if (m_fCameraZoom > 10.f)
		{
			m_fCameraZoom = 10.f;
			return;
		}

		_vector offsetVec = m_pCameraCom->Get_Offset();

		_float originDis = XMVector3Length(XMLoadFloat3(&m_OriginOffset)).m128_f32[0];
		//_float distance = XMVector3Length(offsetVec).m128_f32[0];

		_vector dirVec = XMVector3Normalize(offsetVec);

		_vector zoomResult = dirVec * (originDis - m_fCameraZoom);
		
		m_pCameraCom->Set_Offset(zoomResult);
	}

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

	m_pCameraCom->Set_Offset(XMLoadFloat3(&m_OriginOffset));
	m_fCameraRotationX = 270.f;
	m_fCameraRotationY = 45.f;

	///* For.Com_Collider_Sphere */
	//CBounding_Sphere::BOUNDINGSPHERE		SphereDesc;

	//SphereDesc.fRadius = 0.5f;
	//SphereDesc.vPosition = _float3(0.f, SphereDesc.fRadius * 0.5f, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
	//	TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom[COLLIDER_SPHERE], &SphereDesc)))
	//	return E_FAIL;

	///* For.Com_Collider_AABB */
	//CBounding_AABB::BOUNDINGBOX AABBDesc;

	//AABBDesc.vExtents = _float3(0.5f, 1.f, 0.5f);
	//AABBDesc.vPosition = _float3(0.f, AABBDesc.vExtents.y, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom[COLLIDER_AABB], &AABBDesc)))
	//	return E_FAIL;

	///* For.Com_Collider_OBB */
	//CBounding_OBB::BOUNDINGBOX OBBDesc;

	//OBBDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	//OBBDesc.vPosition = _float3(0.f, OBBDesc.vExtents.y, 0.f);
	//OBBDesc.vRotation = _float3(0.f, XMConvertToRadians(45.0f), 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom[COLLIDER_OBB], &OBBDesc)))
	//	return E_FAIL;
	
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

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCameraCom);

}
