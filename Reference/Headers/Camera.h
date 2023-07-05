#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
	friend class CCamera_Manager;
public:
	typedef struct tagCameraDesc
	{
		_float4			vEye, vAt, vUp;
		_float			fFovy, fAspect, fNear, fFar;
		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMERADESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float TimeDelta) override;
	virtual void	Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Shake(const _float& TimeDelta) {};

	void			Look_At(_fvector pos);
	void			Set_Pos(_fvector pos);
	_vector			Get_Pos();

	void			On_Camera() { m_bCameraOn = true; }
	void			Off_Camera() { m_bCameraOn = false; }

	virtual void			On_Shake(void* pArg = nullptr) { m_bShake = true; }
	virtual void			Off_Shake() { m_bShake = false; }
	
	_bool			Is_Shacking() const { return m_bShake; }
	_bool			Is_On_Camera() const { return m_bCameraOn; }


protected:
	class CTransform*			m_pTransform = { nullptr };
	_float4						m_vEye, m_vAt, m_vUp;
	_float						m_fFovy, m_fAspect, m_fNear, m_fFar;

	class CPipeLine*			m_pPipeLine = { nullptr };

	_bool						m_bCameraOn = { false };
	_bool						m_bShake;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END 