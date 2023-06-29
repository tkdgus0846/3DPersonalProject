#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer, public IReadable
{
public:
	CMesh() : CVIBuffer(nullptr, nullptr) {}
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);

	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	void Get_Matrices(CModel::BONES Bones, _float4x4* pMatrices, _fmatrix PivotMatrix);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const CModel::BONES& Bones, ParsingData* pData, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	char* GetszName() { return m_szName; }
	_uint GetBoneNum() const { return m_iNumBones; }

private:
	char			m_szName[MAX_PATH] = "";
	_uint			m_iMaterialIndex = { 0 };

private:
	_uint			m_iNumBones = { 0 };
	vector<_uint>	m_BoneIndices; /* ∏µ®ø° ∑ŒµÂ«ÿ≥ı¿∫ ¿¸√ºª¿¿« ¿Œµ¶Ω∫. */

private:
	HRESULT Ready_VertexBuffer_NonAnim(MeshParsingData* pData, _fmatrix PivotMatrix);
	HRESULT Ready_VertexBuffer_Anim(MeshParsingData* pData, const CModel::BONES& Bones);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const CModel::BONES& Bones, ParsingData* pData, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	virtual ParsingData* Load_Data(HANDLE handle, ParsingData* data) final;
};

END