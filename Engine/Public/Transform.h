#pragma once

#include "Component.h"

/* 객체의 월드 변환 상태를 가진다.(월드행렬을 보관한다.) */
/* 월드에서의 변환 기능을 가진다.(Go_Straight(), Go_Backword(), Turn() )*/

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent, public ISerializable
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
	enum AXIS { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END};

public:
	typedef struct tagTransformDesc
	{
		tagTransformDesc() = default;
		tagTransformDesc(_double _SpeedPerSec, _double _RoationPerSec)
		: SpeedPerSec { _SpeedPerSec }
		, RotationPerSec { _RoationPerSec }
		{

		}

		_double		SpeedPerSec = { 0.0 };
		_double		RotationPerSec = { 0.0 };
	}TRANSFORMDESC;


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE _eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[_eState];
	}
	_float3 Get_Scaled();
	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}
	void Set_WorldFloat4x4(const _float4x4& matrix) { m_WorldMatrix = matrix; }

	_matrix Get_WorldMatrix_Inverse() const {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_State(STATE _eState, _fvector _vState);
	void Set_Desc(const TRANSFORMDESC & TransformDesc) {
		m_TransformDesc = TransformDesc;
	}
	_float3 Get_Rotation_XYZ() { return m_Rotation; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Set_Position(_fvector vPos);
	void Go_Straight(_double TimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Go_Up(_double TimeDelta);
	void Go_Down(_double TimeDelta);
	_bool Chase(_fvector vTargetPosition, _double TimeDelta, class CNavigation* pNavigation = nullptr, _float fMinDistance = 0.1f);
	void LookAt(_fvector vTargetPosition);
	void Rotation(_fvector vAxis, _float fDegree);
	void Rotation(AXIS eAxis, _float fDegree);
	void Turn(_fvector vAxis, _double TimeDelta);

	void Scaled(const _float3 & vScale);

	


private:
	TRANSFORMDESC			m_TransformDesc;

	_float4x4				m_WorldMatrix;

	// 툴 상에 수치상으로만 각도를 나타내주기 위함. 다른 용도는 없음.
	_float3					m_Rotation = { 0.f,0.f,0.f };

public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	virtual ParsingData* Save_Data(HANDLE handle, ParsingData* data);
	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data);
};

END