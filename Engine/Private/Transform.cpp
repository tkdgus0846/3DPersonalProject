#include "..\Public\Transform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_TransformDesc(0.0, 0.0)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_TransformDesc(rhs.m_TransformDesc)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_float3 CTransform::Get_Scaled()
{
	return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_RIGHT])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_UP])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_LOOK])));
}

void CTransform::Set_State(STATE _eState, _fvector _vState)
{
	_float4		vState;

	XMStoreFloat4(&vState, _vState);

	memcpy(&m_WorldMatrix.m[_eState][0], &vState, sizeof vState);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memmove(&m_TransformDesc, pArg, sizeof m_TransformDesc);

	return S_OK;
}

void CTransform::Set_Position(_fvector vPos)
{
	Set_State(STATE_POSITION, vPos);
}

void CTransform::Go_Straight(_double TimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_double TimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_double TimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_double TimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Chase(_fvector vTargetPosition, _double TimeDelta, _float fMinDistance)
{
	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vDir = vTargetPosition - vPosition;

	if (fMinDistance < XMVectorGetX(XMVector3Length(vDir)))
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.SpeedPerSec * TimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::LookAt(_fvector vTargetPosition)
{
	_float3		vScaled = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = XMVector3Normalize(vTargetPosition - vPosition) * vScaled.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScaled.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScaled.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

/* 항등상태를 기준으로 지정한 각만큼 회전시킨다. */
void CTransform::Rotation(_fvector vAxis, _float fDegree)
{
	_float fRadian = XMConvertToRadians(fDegree);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	/*XMVector3TransformCoord();*/
	/*XMVector4Transform();*/
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(AXIS eAxis, _float fDegree)
{
	_vector vAxis;
	switch(eAxis)
	{
	case AXIS_X:
		vAxis = { 1.f,0.f,0.f,0.f };
		m_Rotation.x = fDegree;
		break;
	case AXIS_Y:
		vAxis = { 0.f,1.f,0.f,0.f };
		m_Rotation.y = fDegree;
		break;
	case AXIS_Z:
		vAxis = { 0.f,0.f,1.f,0.f };
		m_Rotation.z = fDegree;
		break;
	default:
		break;
	}

	_float fRadian = XMConvertToRadians(fDegree);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.RotationPerSec * TimeDelta);

	/*XMVector3TransformCoord();*/
	/*XMVector4Transform();*/
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Scaled(const _float3& vScale)
{
	Set_State(STATE_RIGHT,
		XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP,
		XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK,
		XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);

}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();


}

ParsingData* CTransform::Save_Data(HANDLE handle, ParsingData* data)
{
	DWORD dwByte = { 0 };

	ObjectParsingData* myData = (ObjectParsingData*)data;
	TransformParsingData transformData;
	
	memcpy(&transformData.WorldMatrix, &m_WorldMatrix, sizeof(m_WorldMatrix));
	WriteFile(handle, &m_WorldMatrix, sizeof(m_WorldMatrix), &dwByte, nullptr);


	myData->TransformData = transformData;
	return nullptr;
}

ParsingData* CTransform::Load_Data(HANDLE handle, ParsingData* data)
{
	DWORD dwByte = { 0 };
	ObjectParsingData* myData = (ObjectParsingData*)data;
	TransformParsingData transformData;

	ReadFile(handle, &m_WorldMatrix, sizeof(m_WorldMatrix), &dwByte, nullptr);
	memcpy(&transformData.WorldMatrix, &m_WorldMatrix, sizeof(m_WorldMatrix));

	myData->TransformData = transformData;
	return nullptr;
}
