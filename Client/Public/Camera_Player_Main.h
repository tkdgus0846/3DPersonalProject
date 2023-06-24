#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Player_Main : public CCamera
{
public:
	typedef struct tagPlayerMainCameraDesc
	{
		_uint						iData = { 0 };
		CCamera::CAMERADESC			CameraDesc;
	}CAMERAMAINPLAYERDESC;

private:
	CCamera_Player_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player_Main(const CCamera_Player_Main& rhs);
	virtual ~CCamera_Player_Main() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void			Set_Offset(_fvector offset);
	_vector			Get_Offset();

private:
	_uint						m_iData = { 0 };
	_float3						m_Offset;

public:
	static CCamera_Player_Main* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

