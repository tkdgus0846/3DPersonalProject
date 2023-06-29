#include "stdafx.h"
#include "Camera_Player_Main.h"
#include "GameInstance.h"

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

void CCamera_Player_Main::Tick(_double TimeDelta)
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

void CCamera_Player_Main::Late_Tick(_double TimeDelta)
{
}

HRESULT CCamera_Player_Main::Render()
{
	return S_OK;
}

void CCamera_Player_Main::Set_Offset(_fvector offset)
{
	XMStoreFloat3(&m_Offset, offset);
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