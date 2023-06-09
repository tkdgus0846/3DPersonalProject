#pragma once

#include "Base.h"

BEGIN(Engine)

/* 빛 정보를 가진다. */

class CLight final : public CBase
{
public:
	enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

	typedef struct tagLightDesc
	{
		TYPE	eType;
		_float4	vDir;
		_float4	vPos;
		_float	fRange;

		_float4	vDiffuse;
		_float4	vAmbient;
		_float4	vSpecular;
	}LIGHTDESC;

private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	const LIGHTDESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	LIGHTDESC					m_LightDesc;

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END