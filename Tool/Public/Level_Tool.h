#pragma once

#include "Level.h"
#include "Tool_Defines.h"

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);

private:
	//HRESULT Ready_Layer_BackGround(const _tchar * pLayerTag);

public:
	static CLevel_Tool* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual void Free() override;
};

