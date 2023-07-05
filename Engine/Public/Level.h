#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize(); /* 이 레벨에서 사용되는 초기화나 객체 생서응ㄹ 관리하단. */

	/* 이 레벨에서 보여주고하는 디버깅적 요소. */
	virtual void Tick(_float TimeDelta);
	virtual void Late_Tick(_float TimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };	

public:
	virtual void Free() override;
};

END