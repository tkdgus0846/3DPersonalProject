#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 1. 스레드를 생성하여 다음 레벨에 필요한 자원들을 미리 생성해 놓는다. */

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	const _tchar* Get_LoadingText() const {
		return m_szLoading;
	}

	_bool Get_Finished() const {
		return m_isFinished;
	}

public:
	HRESULT Initialize(LEVELID eNextLevel);
	HRESULT Loading();

private:
	HANDLE				m_hThread = { 0 };
	CRITICAL_SECTION	m_Critical_Section;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	LEVELID				m_eNextLevelID = { LEVEL_END };
	_tchar				m_szLoading[MAX_PATH] = TEXT("");
	_bool				m_isFinished = { false };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free() override;
};

END