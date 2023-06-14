#pragma once

#include "AssimpModel.h"

class CAssimpBone : public IWriteable
{
public:
	CAssimpBone();
	CAssimpBone(const CAssimpBone& rhs);
	virtual ~CAssimpBone() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

	_float4x4 Get_OffsetMatrix() const {
		return m_OffsetMatrix;
	}

	_float4x4 Get_CombinedTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

	void Set_OffsetMatrix(const _float4x4& OffsetMatrix) {
		m_OffsetMatrix = OffsetMatrix;
	}
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(aiNode * pAINode, CAssimpBone * pParent, _uint iIndex);
	void Invalidate_CombinedTransformationMatrix(const CAssimpModel::BONES& Bones);
private:
	char			m_szName[MAX_PATH] = "";
	_float4x4		m_TransformationMatrix;
	_float4x4		m_CombinedTransformationMatrix;
	_float4x4		m_OffsetMatrix;
	// CBone*			m_pParent = { nullptr };
	_int			m_iParentIndex = { -1 };
	_uint			m_iIndex = { 0 };

public:
	static CAssimpBone* Create(aiNode* pAINode, CAssimpBone* pParent, _uint iIndex);


	// IWriteable을(를) 통해 상속됨
	virtual HRESULT Save_Data(ParsingData* data) override;

};
