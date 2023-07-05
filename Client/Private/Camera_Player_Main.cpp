#include "stdafx.h"
#include "Camera_Player_Main.h"
#include "GameInstance.h"

#include <random>

CCamera_Player_Main::CCamera_Player_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player_Main::CCamera_Player_Main(const CCamera_Player_Main& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player_Main::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Player_Main::Initialize(void* pArg)
{
	CAMERAMAINPLAYERDESC		CameraFreeDesc = *(CAMERAMAINPLAYERDESC*)pArg;

	if (nullptr != pArg)
	{
		m_iData = CameraFreeDesc.iData;

		if (FAILED(__super::Initialize(&CameraFreeDesc.CameraDesc)))
			return E_FAIL;
	}



	return S_OK;
}

void CCamera_Player_Main::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);
	

	if (m_bCameraOn == false)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector playerPos = dynamic_cast<CTransform*>(m_pOwner->Get_Component(TRANSFORM_W))->Get_State(CTransform::STATE_POSITION);

	Set_Pos(playerPos + XMLoadFloat3(&m_Offset));
	Look_At(playerPos);


	Safe_Release(pGameInstance);
}

void CCamera_Player_Main::Late_Tick(_float TimeDelta)
{
}

HRESULT CCamera_Player_Main::Render()
{
	return S_OK;
}

void CCamera_Player_Main::Shake(const _float& TimeDelta)
{
	// 오리진 포스가 바뀐경우를 체크

	if (m_fShakeTimeAcc == 0.0f)
		m_ShakeOriginOffset = m_Offset;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

	// 경과 시간 업데이트
	m_fShakeTimeAcc += TimeDelta; // 시간 간격을 조정하여 폭발 지속 시간을 조절하세요

	// 폭발 효과 계산
	float shakeMagnitude = 0.0f;
	if (m_fShakeTimeAcc < m_fShakeDuration)
	{
		float normalizedTime = m_fShakeTimeAcc / m_fShakeDuration;
		shakeMagnitude = m_fShakeMagnitude * (1.0f - std::sqrt(normalizedTime));
	}
	else
	{
		m_Offset = m_ShakeOriginOffset;
		m_bShake = false;
		return;
	}

	// 진동 벡터 생성
	XMVECTOR shakeVector = XMVectorSet(dis(gen), dis(gen), dis(gen), 0.0f);

	// 진동의 크기 조절
	shakeVector *= shakeMagnitude;

	// 카메라 위치에 진동 벡터 더하기
	_vector myOffset = XMVectorSet(m_ShakeOriginOffset.x, m_ShakeOriginOffset.y, m_ShakeOriginOffset.z, 1.f);

	myOffset += shakeVector;

	XMStoreFloat3(&m_Offset, myOffset);
	//Set_Offset(myOffset);
}

void CCamera_Player_Main::On_Shake(void* pArg)
{
	__super::On_Shake(pArg);
	
	MAINCAMERASHAKE* shakeInfo = (MAINCAMERASHAKE*)pArg;

	m_fShakeRadius = shakeInfo->fShakeRadius;
	m_fShakeMagnitude = shakeInfo->fShakeMagnitude;
	m_fShakeDuration = shakeInfo->fShakeDuration;
	m_fShakeTimeAcc = shakeInfo->fShakeTimeAcc;
}

void CCamera_Player_Main::Set_Offset(_fvector offset)
{
	_vector diff = XMVectorSubtract(offset, XMLoadFloat3(&m_Offset));
	XMStoreFloat3(&m_Offset, offset);
	_vector origin = XMLoadFloat3(&m_ShakeOriginOffset);
	XMStoreFloat3(&m_ShakeOriginOffset, origin + diff);

	/*if (m_bShake == false)
		XMStoreFloat3(&m_Offset, offset);
	else
		XMStoreFloat3(&m_ShakeOriginOffset, offset);*/
}

_vector CCamera_Player_Main::Get_Offset()
{
	return XMVectorSet(m_Offset.x, m_Offset.y, m_Offset.z, 1.f);
}

CCamera_Player_Main* CCamera_Player_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player_Main* pInstance = new CCamera_Player_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Player_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject* CCamera_Player_Main::Clone(void* pArg)
{
	CCamera_Player_Main* pInstance = new CCamera_Player_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Player_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Player_Main::Free()
{
	__super::Free();
}