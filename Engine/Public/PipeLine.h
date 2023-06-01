#pragma once

#include "Base.h"

/* ��, ���������� �մ´�. */

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
public:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(D3DTRANSFORMSTATE eTransformState, _fmatrix TransformStateMatrix) {
		XMStoreFloat4x4(&m_TransformState[eTransformState], TransformStateMatrix);
	}
	_matrix Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState);
	_float4x4 Get_TransformFloat4x4(D3DTRANSFORMSTATE eTransformState);
	_matrix Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState);
	_float4x4 Get_TransformFloat4x4_Inverse(D3DTRANSFORMSTATE eTransformState);
	_float4 Get_CamPosition() const {
		return m_vCameraPos;
	}
public:
	void Tick();

private:
	_float4x4				m_TransformState[D3DTS_END];
	_float4x4				m_TransformState_Inverse[D3DTS_END];
	_float4					m_vCameraPos;

public:
	virtual void Free() override;
};

END