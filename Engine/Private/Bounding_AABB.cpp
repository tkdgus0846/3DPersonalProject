#include "..\Public\Bounding_AABB.h"

#include "Bounding_OBB.h"
#include "Bounding_SPHERE.h"

CBounding_AABB::CBounding_AABB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding(pDevice, pContext)
{

}

CBounding_AABB::CBounding_AABB(const CBounding_AABB & rhs)
	: CBounding(rhs)
	, m_pAABB_Original(new BoundingBox(*rhs.m_pAABB_Original))
	, m_pAABB(new BoundingBox(*m_pAABB_Original))
{

}

HRESULT CBounding_AABB::Initialize_Prototype()
{
	m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));

	return S_OK;
}

HRESULT CBounding_AABB::Initialize(void * pBoundingDesc)
{
	m_pAABB_Original->Extents = ((BOUNDINGBOX*)pBoundingDesc)->vExtents;
	m_pAABB_Original->Center = ((BOUNDINGBOX*)pBoundingDesc)->vPosition;
	
	*m_pAABB = *m_pAABB_Original;

	return S_OK;

}

void CBounding_AABB::Tick(_fmatrix TransformMatrix)
{
	m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pAABB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eType, const CBounding* pBounding)
{

	m_isColl = { false };

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pAABB->Intersects(*((CBounding_AABB*)pBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		m_isColl = m_pAABB->Intersects(*((CBounding_OBB*)pBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_SPHERE:
		m_isColl = m_pAABB->Intersects(*((CBounding_Sphere*)pBounding)->Get_Bounding());
		break;
	}

	return m_isColl;
}

_matrix CBounding_AABB::Remove_Rotation(_fmatrix TransformMatrix)
{	
	_matrix		ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

CBounding_AABB * CBounding_AABB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBounding_AABB*	pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_AABB");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBounding * CBounding_AABB::Clone(void * pBoundingDesc)
{
	CBounding_AABB*	pInstance = new CBounding_AABB(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_AABB");
		Safe_Release(pInstance);
	}
	return pInstance;

	
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
}

