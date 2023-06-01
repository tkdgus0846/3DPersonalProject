#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

_matrix CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eTransformState)
{
	return XMLoadFloat4x4(&m_TransformState[eTransformState]);	
}

_float4x4 CPipeLine::Get_TransformFloat4x4(D3DTRANSFORMSTATE eTransformState)
{
	return m_TransformState[eTransformState];
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return XMLoadFloat4x4(&m_TransformState_Inverse[eTransformState]);

}

_float4x4 CPipeLine::Get_TransformFloat4x4_Inverse(D3DTRANSFORMSTATE eTransformState)
{
	return m_TransformState_Inverse[eTransformState];
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < D3DTS_END; ++i)
	{
		XMStoreFloat4x4(&m_TransformState_Inverse[i], XMMatrixInverse(nullptr, Get_TransformMatrix(D3DTRANSFORMSTATE(i))));
	}
	
	memcpy(&m_vCameraPos, &m_TransformState_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

void CPipeLine::Free()
{
}
