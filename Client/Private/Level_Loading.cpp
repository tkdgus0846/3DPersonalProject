#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;

	/* 스레드를 생성한다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float TimeDelta)
{
	__super::Tick(TimeDelta);

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (false == m_pLoader->Get_Finished())
			return;

		CLevel*		pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
		{
			Safe_Release(pGameInstance);
			return;
		}

		Safe_Release(pGameInstance);

		return;
	}

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
}

void CLevel_Loading::Late_Tick(_float TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}
