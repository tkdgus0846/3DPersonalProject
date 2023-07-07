#pragma once

#include "Tool_Defines.h"
#include "Camera.h"


class CCamera_Main final : public CCamera
{
public:
	enum TYPE
	{
		TYPE_MAIN, TYPE_OBJECT, TYPE_ANIM, TYPE_END
	};
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
	virtual void Tick(_float TimeDelta) override;
	virtual _int Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;

	void ChooseType(TYPE type, _fmatrix matrix = XMMatrixIdentity());

private:
	_uint						m_iData = { 0 };

	_float4x4					m_MatrixArr[TYPE_END];
	
	TYPE						m_eCurType = TYPE_MAIN;

public:
	static CCamera_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CCamera을(를) 통해 상속됨
	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data) override;
	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) override;
};

