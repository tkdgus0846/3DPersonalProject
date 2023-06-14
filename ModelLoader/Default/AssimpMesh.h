#pragma once

#include "AssimpModel.h"

class CAssimpMesh : public IWriteable
{
public:
	CAssimpMesh();
	~CAssimpMesh();

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}


public:
	virtual HRESULT Initialize_Prototype(const CAssimpModel::BONES& Bones, const aiMesh* pAIMesh, _fmatrix PivotMatrix);

private:
	char			m_szName[MAX_PATH] = "";
	_uint			m_iMaterialIndex = { 0 };

	_uint			m_iNumVertices = { 0 };
	_uint			m_iNumIndices = { 0 };
	vector<_ulong>		m_Indices;
	vector<VTXANIMMESH>	m_Vertices;

	_uint			m_iNumBones = { 0 };
	vector<_uint>	m_BoneIndices; /* �𵨿� �ε��س��� ��ü���� �ε���. */
private:
	HRESULT Ready_VertexBuffer_Anim(const aiMesh* pAIMesh, const CAssimpModel::BONES& Bones);

public:
	static CAssimpMesh* Create(const CAssimpModel::BONES& Bones, const aiMesh* pAIMesh, _fmatrix PivotMatrix);

	// ISerializable��(��) ���� ��ӵ�
	virtual HRESULT Save_Data(ParsingData* data) override;
	
};
