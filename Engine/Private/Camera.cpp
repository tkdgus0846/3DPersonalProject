#include "..\Public\Camera.h"
#include "PipeLine.h"


CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	if (nullptr != pArg)
	{
		CAMERADESC	CameraDesc = *(CAMERADESC*)pArg;

		m_vEye = CameraDesc.vEye;
		m_vAt = CameraDesc.vAt;
		m_vUp = CameraDesc.vUp;

		m_fFovy = CameraDesc.fFovy;
		m_fAspect = CameraDesc.fAspect;
		m_fNear = CameraDesc.fNear;
		m_fFar = CameraDesc.fFar;

		m_pTransform->Set_Desc(CameraDesc.TransformDesc);
	}

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_vAt));

	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Late_Tick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Look_At(_fvector pos)
{
	XMStoreFloat4(&m_vAt, pos);
	m_pTransform->LookAt(pos);
}

void CCamera::Set_Pos(_fvector pos)
{
	XMStoreFloat4(&m_vEye, pos);
	m_pTransform->Set_Position(pos);
}

_vector CCamera::Get_Pos()
{
	return XMLoadFloat4(&m_vEye);
}

void CCamera::On_Shake(SHAKE_TYPE eType, const _float& fForce, const _float& fTime)
{
	m_eType = eType;
	m_bShake = true;
	m_fShakeForce = fForce;
	m_fShakeTime = fTime;
}


void CCamera::Free()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTransform);
	__super::Free();
}
