#pragma once

#include "Tool_Defines.h"
#include "Camera.h"


class CCamera_Main final : public CCamera
{
public:
	typedef struct tagFreeCameraDesc
	{
		_uint						iData = { 0 };
		CCamera::CAMERADESC			CameraDesc;
	}CAMERAFREEDESC;

private:
	CCamera_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Main(const CCamera_Main& rhs);
	virtual ~CCamera_Main() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint						m_iData = { 0 };

public:
	static CCamera_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

