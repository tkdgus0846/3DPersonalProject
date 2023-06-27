#include "..\Public\Bounding_OBB.h"

#include "Bounding_AABB.h"
#include "Bounding_SPHERE.h"

CBounding_OBB::CBounding_OBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding(pDevice, pContext)
{

}

CBounding_OBB::CBounding_OBB(const CBounding_OBB & rhs)
	: CBounding(rhs)
	, m_pOBB_Original(new BoundingOrientedBox(*rhs.m_pOBB_Original))
	, m_pOBB(new BoundingOrientedBox(*m_pOBB_Original))
{

}

HRESULT CBounding_OBB::Initialize_Prototype()
{
	m_pOBB_Original = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CBounding_OBB::Initialize(void * pBoundingDesc)
{
	m_pOBB_Original->Extents = ((BOUNDINGBOX*)pBoundingDesc)->vExtents;
	m_pOBB_Original->Center = ((BOUNDINGBOX*)pBoundingDesc)->vPosition;
	XMStoreFloat4(&m_pOBB_Original->Orientation, XMQuaternionRotationRollPitchYaw(((BOUNDINGBOX*)pBoundingDesc)->vRotation.x, ((BOUNDINGBOX*)pBoundingDesc)->vRotation.y, ((BOUNDINGBOX*)pBoundingDesc)->vRotation.z));	

	*m_pOBB = *m_pOBB_Original;

	return S_OK;

}

void CBounding_OBB::Tick(_fmatrix TransformMatrix)
{
	m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pOBB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, const CBounding * pBounding)
{

	m_isColl = { false };

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pOBB->Intersects(*((CBounding_AABB*)pBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		m_isColl = m_pOBB->Intersects(*((CBounding_OBB*)pBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_SPHERE:
		m_isColl = m_pOBB->Intersects(*((CBounding_Sphere*)pBounding)->Get_Bounding());
		break;
	}

	return m_isColl;
}

CBounding_OBB::OBBDESC CBounding_OBB::Compute_OBBDesc()
{
	CBounding_OBB::OBBDESC		OBBDesc;

	OBBDesc.vCenter = m_pOBB->Center;

	_float3		vPoints[8];

	m_pOBB->GetCorners(vPoints);

	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoints[2]) - XMLoadFloat3(&vPoints[3])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoints[2]) - XMLoadFloat3(&vPoints[1])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoints[2]) - XMLoadFloat3(&vPoints[6])) * 0.5f);

	for (size_t i = 0; i < 3; i++)
		XMStoreFloat3(&OBBDesc.vAlignAxis[i], 
			XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[i])));

	return OBBDesc;
}


CBounding_OBB * CBounding_OBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBounding_OBB*	pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_OBB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBounding * CBounding_OBB::Clone(void * pBoundingDesc)
{
	CBounding_OBB*	pInstance = new CBounding_OBB(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_OBB");
		Safe_Release(pInstance);
	}
	return pInstance;

	
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
}
