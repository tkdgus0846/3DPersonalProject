#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	enum TYPE
	{
		DYNAMIC, STATIC
	};
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMap, TYPE eType);
	HRESULT Initialize_Prototype(const _uint & sizeX, const _uint & sizeZ, const _float& fInterval, TYPE eType);
	HRESULT Initialize_Prototype(const wstring& pCustomHeightMap, TYPE eType);

	void			Culling(_fmatrix WorldMatrix, _bool bLOD = true);

	virtual HRESULT Initialize(void* pArg) override;
	vector<Triangle>* Get_VerticesPos() { return &m_VerticesPos; }

	void			Picking_Height_Cos(_uint index, _float radius, _float power, _fvector centerPos);
	void			Backing_Normal_All();

private:
	void			Compute_Normal(const _uint* verticesIndex, VTXPOSNORTEX* pVertices);
	_float			Compute_Height(_fvector originPos, _float radius, _float power, _fvector centerPos);
	_vector			Compute_Pos(_uint index);
	_float			Compute_Distance(_fvector aPos, _fvector bPos);

private:
	_uint				m_iNumVerticesX = { 0 };
	_uint				m_iNumVerticesZ = { 0 };
	_float				m_fInterval = { 1.f };

	vector<Triangle>	m_VerticesPos;
	_ulong*				m_pIndices = { nullptr };
	class CQuadTree*	m_pQuadTree = { nullptr };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMap, TYPE eType = STATIC);
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pCustomHeightMap, TYPE eType = STATIC);
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _uint& sizeX, const _uint& sizeZ, const _float& fInterval = 1.f, TYPE eType = DYNAMIC);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	void Save_TerrainBuffer(const wstring& name);
};

END