#include "stdafx.h"
#include "Camera_Main.h"
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

		//ChooseType(TYPE_MAIN);
		CGameInstance::GetInstance()->On_Camera(this);
	}

	

	return S_OK;
}

void CCamera_Main::Tick(_float TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_long		MouseMove = { 0 };

	if (pGameInstance->Mouse_Pressing(CInput_Device::DIMK_RBUTTON))
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

void CCamera_Main::Late_Tick(_float TimeDelta)
{
}

HRESULT CCamera_Main::Render()
{
	return S_OK;
}

void CCamera_Main::ChooseType(TYPE type, _fmatrix matrix)
{
	if (m_eCurType == TYPE_MAIN && type == TYPE_MAIN) return;

	if (m_eCurType == TYPE_MAIN && type != TYPE_MAIN)
		m_MatrixArr[m_eCurType] = m_pTransform->Get_WorldFloat4x4();

	m_eCurType = type;

	// m_MatrixArr[m_eCurType] 에 채워주고 
	if (type == TYPE_OBJECT)
	{
		_float4x4 objectMatrix;
		matrix; // 가공해서 집어넣기
	
		_vector vEye = XMVectorSubtract(matrix.r[3], XMVectorSet(0.f, -10.f, 10.f, 0.f));
		_vector vAt = matrix.r[3];
		
		m_pTransform->Set_State(CTransform::STATE_POSITION, vEye);
		m_pTransform->LookAt(vAt, CTransform::AIRCRAFT);

		m_MatrixArr[m_eCurType] = m_pTransform->Get_WorldFloat4x4();
	}
	if (type == TYPE_ANIM)
	{
		_vector vEye = { -100.f, -10099.f, -110.f, 1.f };
		_vector vAt = { -100.f, -10099.f, -100.f, 1.f };

		m_pTransform->Set_State(CTransform::STATE_POSITION, vEye);
		m_pTransform->LookAt(vAt, CTransform::AIRCRAFT);

		m_MatrixArr[m_eCurType] = m_pTransform->Get_WorldFloat4x4();
	}
	else
	{
		m_pTransform->Set_WorldFloat4x4(m_MatrixArr[m_eCurType]);
	}

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

ParsingData* CCamera_Main::Save_Data(HANDLE handle, ParsingData* data)
{
	return nullptr;
}

ParsingData* CCamera_Main::Load_Data(HANDLE handle, ParsingData* data)
{
	return nullptr;
}
