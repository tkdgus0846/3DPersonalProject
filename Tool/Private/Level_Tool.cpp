#include "stdafx.h"
#include "..\Public\Level_Tool.h"

#include "GameInstance.h"
#include "Camera_Main.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	/*if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyObject"), TEXT("Layer_Object"), TEXT("EmptyObject"))))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;*/

	return S_OK;
}

void CLevel_Tool::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLevel_Tool::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	SetWindowText(g_hWnd, TEXT("상현이툴"));
}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Main::CAMERAFREEDESC		CameraFreeDesc;

	CameraFreeDesc.iData = { 0 };
	CameraFreeDesc.CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraFreeDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraFreeDesc.CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraFreeDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraFreeDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraFreeDesc.CameraDesc.fNear = 0.2f;
	CameraFreeDesc.CameraDesc.fFar = 300.f;
	CameraFreeDesc.CameraDesc.TransformDesc.SpeedPerSec = 10.f;
	CameraFreeDesc.CameraDesc.TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	wstring name = TEXT("MainCamera");
	if (pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera_Main"), pLayerTag, name, &CameraFreeDesc) == nullptr)
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Tool* CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool* pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();

}
