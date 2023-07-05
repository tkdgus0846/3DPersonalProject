#pragma once

#include "Tool_Defines.h"
#include "Base.h"




BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Tool)

class CToolApp final : public CBase
{
private:
	CToolApp();
	virtual ~CToolApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float TimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CRenderer* m_pRenderer = { nullptr };


public:
	HRESULT Ready_Prototype_Component_For_Static();
	/* 최초 보여 줄 레벨을 결정한다. */
	HRESULT Open_Level(LEVELID eLevelIndex);



public:
	static CToolApp* Create();
	virtual void Free() override;
};

END
