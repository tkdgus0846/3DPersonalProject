#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMap);
	HRESULT Initialize_Prototype(const _uint & sizeX, const _uint & sizeZ, const _float& fInterval);
	virtual HRESULT Initialize(void* pArg) override;
	
private:
	void			Compute_Normal(const _uint* verticesIndex, VTXPOSNORTEX* pVertices);

private:
	_uint			m_iNumVerticesX = { 0 };
	_uint			m_iNumVerticesZ = { 0 };
	_float			m_fInterval = { 1.f };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMap);
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _uint& sizeX, const _uint& sizeZ, const _float& fInterval = 1.f);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END