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

	typedef struct PlayerMainCameraShakeDesc
	{
		_float	fShakeTimeAcc = { 0.f };
		_float	fShakeRadius = { 10.f };
		_float	fShakeMagnitude = { 1.2f };
		_float	fShakeDuration = { 1.f };
	}MAINCAMERASHAKE;

private:
	CCamera_Player_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player_Main(const CCamera_Player_Main& rhs);
	virtual ~CCamera_Player_Main() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Shake(const _double& TimeDelta) override;

	virtual void	On_Shake(void* pArg = nullptr) override;

	void			Set_Offset(_fvector offset);
	_vector			Get_Offset();

private:
	_uint						m_iData = { 0 };
	_float3						m_Offset;
	_float3						m_ShakeOriginOffset;

	_float						m_fShakeTimeAcc = { 0.f };
	_float						m_fShakeRadius = { 10.f };
	_float						m_fShakeMagnitude = { 1.2f };
	_float						m_fShakeDuration = { 1.f };

public:
	static CCamera_Player_Main* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

