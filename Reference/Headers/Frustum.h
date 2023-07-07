#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Tick();

	void		Transform_ToLocalSpace(_fmatrix WorldMatrix);
	_bool		isIn_WorldSpace(_fvector vWorldPos, _float fRange);
	_bool		isIn_LocalSpace(_fvector vLocalPos, _float fRange);

private:
	_float3			m_vOriginal_Points[8];
	_float4			m_vWorldPlanes[6];
	_float4			m_vLocalPlanes[6];
	
public:
	virtual void Free() override;
};

END