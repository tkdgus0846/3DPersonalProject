#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 이전 레벨에서 사용되던 자원을 날린다. */
	if(nullptr != m_pCurrentLevel)
		pGameInstance->Clear_LevelResources(m_iLevelIndex);

	Safe_Release(pGameInstance);

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	m_iLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Tick(_float TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(TimeDelta);
}

void CLevel_Manager::Late_Tick(_float TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Late_Tick(TimeDelta);
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}

