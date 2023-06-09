#pragma once

#include "Client_Defines.h"
#include "Base.h"




BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)
	
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float TimeDelta);
	HRESULT Render();
	HRESULT PostRender();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	

public: 
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Gara();
	/* 최초 보여 줄 레벨을 결정한다. */
	HRESULT Open_Level(LEVELID eLevelIndex);
	
#ifdef _DEBUG
private:
	_double					m_TimeAcc = { 0.0 };
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_uint					m_iNumRenders = { 0 };
#endif

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END