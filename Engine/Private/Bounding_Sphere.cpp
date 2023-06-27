#include "..\Public\Bounding_Sphere.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBounding(pDevice, pContext)
{

}

CBounding_Sphere::CBounding_Sphere(const CBounding_Sphere & rhs)
	: CBounding(rhs)
	, m_pSphere_Original(new BoundingSphere(*rhs.m_pSphere_Original))
	, m_pSphere(new BoundingSphere(*m_pSphere_Original))
{

}

HRESULT CBounding_Sphere::Initialize_Prototype()
{
	m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);

	return S_OK;
}

HRESULT CBounding_Sphere::Initialize(void * pBoundingDesc)
{
	m_pSphere_Original->Radius = ((BOUNDINGSPHERE*)pBoundingDesc)->fRadius;
	m_pSphere_Original->Center = ((BOUNDINGSPHERE*)pBoundingDesc)->vPosition;

	*m_pSphere = *m_pSphere_Original;

	return S_OK;

}

void CBounding_Sphere::Tick(_fmatrix TransformMatrix)
{
	m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pSphere, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eType, const CBounding * pBounding)
{
	m_isColl = { false };

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pSphere->Intersects(*((CBounding_AABB*)pBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_OBB:
		m_isColl = m_pSphere->Intersects(*((CBounding_OBB*)pBounding)->Get_Bounding());
		break;
	case CCollider::TYPE_SPHERE:
		m_isColl = m_pSphere->Intersects(*((CBounding_Sphere*)pBounding)->Get_Bounding());
		break;
	}

	return m_isColl;
}



CBounding_Sphere * CBounding_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBounding_Sphere*	pInstance = new CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBounding * CBounding_Sphere::Clone(void * pBoundingDesc)
{
	CBounding_Sphere*	pInstance = new CBounding_Sphere(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;

	
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
}
