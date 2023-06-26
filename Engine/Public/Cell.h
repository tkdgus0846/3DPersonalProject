#pragma once

#include "Base.h"


BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END	};
	enum OPTION { FALL, NOPASS, NORMAL};
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void SetUp_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor) {
		m_iNeighborIndices[eNeighbor] = pNeighbor->m_iIndex;
	}

	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_bool is_In(_fvector vPosition, _int* pNeighborIndex, _int* neighbor = nullptr);
	
	void Set_Option(OPTION eOption) { m_eOption = eOption; }
	OPTION Get_Option() const { return m_eOption; }

	_vector Get_PlaneNormal() const { return XMLoadFloat3(&m_vPlaneNormal); }
	_vector Get_Normal(_int neighbor) { return XMLoadFloat3(&m_vNormals[neighbor]); }
	_int SlidingVector(_uint& index, _fvector pos, _fvector dir, _float3& slidingVector, map<_uint, _uint>& history);


private:
	void Sort_Points_CW(const _float3* pPoints, _float3* sortedPoints);
	void Calculate_PlaneNormal();
	



#ifdef _DEBUG
public:
	HRESULT Render();
	HRESULT Render_BoundingSpheres(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch);

	void	Reset_Selected();
	_float3	Select_Point(_uint point);
	_float3	CancelSelect_Point(_uint point);
	_float3 Picking_Point(_uint point);

	vector<BoundingSphere> Get_All_Sphere();
	void	Translate(_uint point, _fvector trans);

private:

#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
private:
	_int				m_iIndex = { 0 };
	_float3				m_vPoints[POINT_END];
	_float3				m_vNormals[NEIGHBOR_END];
	_int				m_iNeighborIndices[NEIGHBOR_END] = { -1, -1, -1 };
	OPTION				m_eOption = NORMAL;
	_float3				m_vPlaneNormal;

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = { nullptr };
	BoundingSphere					m_BoundingSphere[POINT_END];
	BoundingSphere					m_OriginBoundingSphere[POINT_END];
	_bool							m_bSelected[POINT_END];
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free();
};

END