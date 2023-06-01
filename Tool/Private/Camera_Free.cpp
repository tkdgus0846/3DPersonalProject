#include "stdafx.h"
#include "Camera_Free.h"
#include "GameInstance.h"

CCamera_Main::CCamera_Main(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Main::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Main::Initialize(void * pArg)
{
	CAMERAFREEDESC		CameraFreeDesc = *(CAMERAFREEDESC*)pArg;

	if (nullptr != pArg)
	{
		m_iData = CameraFreeDesc.iData;

		if (FAILED(__super::Initialize(&CameraFreeDesc.CameraDesc)))
			return E_FAIL;
	}

	

	return S_OK;
}

void CCamera_Main::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_long		MouseMove = { 0 };

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON))
	{
		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
		{
			m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * TimeDelta * 0.2f);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y))
		{
			m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), MouseMove * TimeDelta * 0.2f);
		}

		if (GetKeyState('W') & 0x8000)
		{
			m_pTransform->Go_Straight(TimeDelta);
		}

		if (GetKeyState('S') & 0x8000)
		{
			m_pTransform->Go_Backward(TimeDelta);
		}

		if (GetKeyState('A') & 0x8000)
		{
			m_pTransform->Go_Left(TimeDelta);
		}

		if (GetKeyState('D') & 0x8000)
		{
			m_pTransform->Go_Right(TimeDelta);
		}

		if (GetKeyState('E') & 0x8000)
		{
			m_pTransform->Go_Up(TimeDelta);
		}

		if (GetKeyState('Q') & 0x8000)
		{
			m_pTransform->Go_Down(TimeDelta);
		}
	}
	

	Safe_Release(pGameInstance);
	
	__super::Tick(TimeDelta);
}

void CCamera_Main::Late_Tick(_double TimeDelta)
{
}

HRESULT CCamera_Main::Render()
{
	return S_OK;
}

CCamera_Main * CCamera_Main::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Main*	pInstance = new CCamera_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCamera_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject * CCamera_Main::Clone(void * pArg)
{
	CCamera_Main*	pInstance = new CCamera_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCamera_Main");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Main::Free()
{
	__super::Free();
}
