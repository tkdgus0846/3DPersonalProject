#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#include "DebugDraw.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	m_iIndex = iIndex;

	_float3 sortedPoints[3];

	Sort_Points_CW(pPoints, sortedPoints);

	

	memcpy(m_vPoints, sortedPoints, sizeof(_float3) * POINT_END);

	_vector		vLine;

	vLine = (XMLoadFloat3(&sortedPoints[POINT_B]) - XMLoadFloat3(&sortedPoints[POINT_A]));
	m_vNormals[NEIGHBOR_AB] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = (XMLoadFloat3(&sortedPoints[POINT_C]) - XMLoadFloat3(&sortedPoints[POINT_B]));
	m_vNormals[NEIGHBOR_BC] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = (XMLoadFloat3(&sortedPoints[POINT_A]) - XMLoadFloat3(&sortedPoints[POINT_C]));
	m_vNormals[NEIGHBOR_CA] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	Calculate_PlaneNormal();

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, sortedPoints, CVIBuffer_Cell::TYPE_DYNAMIC);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_OriginBoundingSphere[POINT_A].Radius = 1.0f;
	m_OriginBoundingSphere[POINT_B].Radius = 1.0f;
	m_OriginBoundingSphere[POINT_C].Radius = 1.0f;

	m_BoundingSphere[POINT_A].Center = sortedPoints[POINT_A];
	m_BoundingSphere[POINT_A].Radius = 1.0f;
	m_BoundingSphere[POINT_B].Center = sortedPoints[POINT_B];
	m_BoundingSphere[POINT_B].Radius = 1.0f;
	m_BoundingSphere[POINT_C].Center = sortedPoints[POINT_C];
	m_BoundingSphere[POINT_C].Radius = 1.0f;

	m_bSelected[POINT_A] = false;
	m_bSelected[POINT_B] = false;
	m_bSelected[POINT_C] = false;

#endif

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
		{
			return true;
		}
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
		{
			return true;
		}
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
		{
			return true;
		}
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
		{
			return true;
		}
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
		{
			return true;
		}
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
		{
			return true;
		}
	}

	return false;
}

_bool CCell::is_In(_fvector vPosition, _int* pNeighborIndex, _int* neighbor)
{
	for (size_t i = 0; i < NEIGHBOR_END; i++)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector		vNormal = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			// 밖에 나간것.
			if (neighbor != nullptr)
			{
				*neighbor = i;
			}
				

			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}
	return true;
}

struct SlideCmpData
{
	_int index;
	_float distance;
	_float dot;
};

_int CCell::SlidingVector(_uint& index, _fvector pos, _fvector dir, _float3& slidingVector, map<_uint, _uint>& history)
{
	/*if (history.find(index) == history.end())
	{
		history[index] = 1;
	}
	else
	{
		history[index]++;
	}*/
	

	vector<SlideCmpData> myVec;

	_vector vec1, vec2, normalVec; 
	_float distance;

	NEIGHBOR eIndex;
	_vector normal;
	_float length;

	for (int i = 0; i < 3; i++)
	{
		_int index = (i + 1 == 3) ? 0 : i + 1;
		vec1 = pos - XMLoadFloat3(&m_vPoints[i]);
		vec2 = pos - XMLoadFloat3(&m_vPoints[index]);
		normalVec = -XMLoadFloat3(&m_vNormals[i]);
		normalVec = XMVector3Normalize(normalVec);

		distance = 0.f;
		distance += fabsf(XMVector3Dot(vec1, normalVec).m128_f32[0]);
		distance += fabsf(XMVector3Dot(vec2, normalVec).m128_f32[0]);

		normal = -XMLoadFloat3(&m_vNormals[i]);
		normal = XMVector3Normalize(normal);
		length = XMVector3Dot(dir, normal).m128_f32[0];

		myVec.push_back({ i, distance, length });
	}

	sort(myVec.begin(), myVec.end(),
		[=](SlideCmpData& a, SlideCmpData& b)
		{
			if (a.distance < b.distance)
			{
				if (b.distance - a.distance < 2.f)
				{
					if (a.dot > b.dot)
					{
						return false;
					}
				}
				
			}
			return a.distance < b.distance;
		}
	);

	eIndex = (NEIGHBOR)myVec[0].index;

	normal = -XMLoadFloat3(&m_vNormals[eIndex]);
	normal = XMVector3Normalize(normal);

	_vector myDir = dir * -1;
	length = XMVector3Dot(myDir, normal).m128_f32[0];

	normal = normal * length;

	XMStoreFloat3(&slidingVector, normal - myDir);
	return 1;
}

void CCell::Sort_Points_CW(const _float3* pPoints, _float3* sortedPoints)
{
	// x-z 상으로 비교해보는 시계방향
	_vector aDot = XMVectorSet(pPoints[POINT_A].x, pPoints[POINT_A].z, 0.f, 0.f);
	_vector bDot = XMVectorSet(pPoints[POINT_B].x, pPoints[POINT_B].z, 0.f, 0.f);
	_vector cDot = XMVectorSet(pPoints[POINT_C].x, pPoints[POINT_C].z, 0.f, 0.f);

	_vector ABVec = XMVector2Normalize(bDot - aDot);
	_vector ACVec = XMVector2Normalize(cDot - aDot);

	_vector crossVec = XMVector2Cross(ABVec, ACVec);

	sortedPoints[POINT_A] = pPoints[POINT_A];
	// 시계 방향
	if (crossVec.m128_f32[2] < 0)
	{
		sortedPoints[POINT_B] = pPoints[POINT_B];
		sortedPoints[POINT_C] = pPoints[POINT_C];
	}
	// 반시계 방향
	else if (crossVec.m128_f32[2] > 0)
	{
		sortedPoints[POINT_B] = pPoints[POINT_C];
		sortedPoints[POINT_C] = pPoints[POINT_B];
	}
	else
	{
		// x나 z 둘중에 하나가 같아져버린 경우임
	}


	// 값이 0이 나왔을 경우엔 x-y로

	// 값이 또 0 이 나왔을 경우엔 z-y로
	 
}

void CCell::Calculate_PlaneNormal()
{
	_vector ADot = XMLoadFloat3(&m_vPoints[POINT_A]);
	_vector BDot = XMLoadFloat3(&m_vPoints[POINT_B]);
	_vector CDot = XMLoadFloat3(&m_vPoints[POINT_C]);

	_vector AB = BDot - ADot;
	_vector BC = CDot - BDot;

	AB = XMVector3Normalize(AB);
	BC = XMVector3Normalize(BC);

	XMStoreFloat3(&m_vPlaneNormal, XMVector3Cross(AB, BC));
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CCell::Render_BoundingSpheres(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	_matrix TransformMatrix = XMMatrixIdentity();
	_vector color;

	/// POINT_A

	if (m_bSelected[POINT_A] == true) 
		color = XMVectorSet(1.f, 0.f, 0.f, 1.f);
	else
		color = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	TransformMatrix.r[3] = XMVectorSet(m_vPoints[POINT_A].x, m_vPoints[POINT_A].y, m_vPoints[POINT_A].z, 1.f);
	m_OriginBoundingSphere[POINT_A].Transform(m_BoundingSphere[POINT_A], TransformMatrix);
	DX::Draw(pBatch, m_BoundingSphere[POINT_A], color);

	/// POINT_B

	if (m_bSelected[POINT_B] == true)
		color = XMVectorSet(1.f, 0.f, 0.f, 1.f);
	else
		color = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	TransformMatrix.r[3] = XMVectorSet(m_vPoints[POINT_B].x, m_vPoints[POINT_B].y, m_vPoints[POINT_B].z, 1.f);
	m_OriginBoundingSphere[POINT_B].Transform(m_BoundingSphere[POINT_B], TransformMatrix);
	DX::Draw(pBatch, m_BoundingSphere[POINT_B], color);

	/// POINT_C

	if (m_bSelected[POINT_C] == true)
		color = XMVectorSet(1.f, 0.f, 0.f, 1.f);
	else
		color = XMVectorSet(0.f, 1.f, 0.f, 1.f);

	TransformMatrix.r[3] = XMVectorSet(m_vPoints[POINT_C].x, m_vPoints[POINT_C].y, m_vPoints[POINT_C].z, 1.f);
	m_OriginBoundingSphere[POINT_C].Transform(m_BoundingSphere[POINT_C], TransformMatrix);
	DX::Draw(pBatch, m_BoundingSphere[POINT_C], color);
	return S_OK;
}

void CCell::Reset_Selected()
{
	for (int i = 0; i < 3; i++)
	{
		m_bSelected[i] = false;
	}
}

_float3 CCell::Select_Point(_uint point)
{
	m_bSelected[point] = true;
	return m_vPoints[point];
}

_float3 CCell::CancelSelect_Point(_uint point)
{
	m_bSelected[point] = false;
	return m_vPoints[point];
}

//  구 - 광선 충돌이 성공 했을경우에 그 광선이 맞은 위치를 
_float3 CCell::Picking_Point(_uint point)
{
	/*_float distance = 999999.f;
	_uint index = -1;

	for (int i = 0; i < POINT_END; i++)
	{
		_vector pointPos = XMVectorSet(m_vPoints[i].x, m_vPoints[i].y, m_vPoints[i].z ,1.f);

		_vector disVec = XMVectorSubtract(pointPos, pos);
		_float dis = XMVector3Length(disVec).m128_f32[0];
		
		index = (dis < distance) ? i : index;
	}*/

	return Select_Point(point);
}

vector<BoundingSphere> CCell::Get_All_Sphere()
{
	vector<BoundingSphere> resultVec;

	for (int i = 0; i < POINT_END; i++)
		resultVec.push_back(m_BoundingSphere[i]);

	return resultVec;
}

// 붙어있는 다른 놈들도 다 움직여줘야됨.
void CCell::Translate(_uint point, _fvector trans)
{
	XMStoreFloat3(&m_BoundingSphere[point].Center, trans);
	XMStoreFloat3(&m_vPoints[point], trans);
	m_pVIBuffer->Edit_Point(point, m_vPoints[point]);

	_vector		vLine;

	vLine = (XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	m_vNormals[NEIGHBOR_AB] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = (XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	m_vNormals[NEIGHBOR_BC] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = (XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	m_vNormals[NEIGHBOR_CA] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));
}

#endif

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
