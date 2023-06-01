#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual CGameObject* Clone(void* pArg) override PURE;

protected:
	_uint		m_iPassNum = { 0 };
	_uint		m_eRenderGroup;
};

END