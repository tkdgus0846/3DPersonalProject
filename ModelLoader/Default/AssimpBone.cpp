#include "stdafx.h"
#include "AssimpBone.h"

CAssimpBone::CAssimpBone()
{
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
}

CAssimpBone::CAssimpBone(const CAssimpBone & rhs)
	: m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_CombinedTransformationMatrix(rhs.m_CombinedTransformationMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_iParentIndex(rhs.m_iParentIndex)
	, m_iIndex(rhs.m_iIndex)
{
	strcpy_s(m_szName, rhs.m_szName);
}


HRESULT CAssimpBone::Initialize(aiNode * pAINode, CAssimpBone * pParent, _uint iIndex)
{
	strcpy_s(m_szName, pAINode->mName.data);
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_OffsetMatrix = m_CombinedTransformationMatrix;
	m_iParentIndex = pParent == nullptr ? -1 : pParent->m_iIndex;
	m_iIndex = iIndex;

	return S_OK;
}

void CAssimpBone::Invalidate_CombinedTransformationMatrix(const CAssimpModel::BONES& Bones)
{
	if (-1 == m_iParentIndex)
	{
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	}
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));

	}

	
}

CAssimpBone * CAssimpBone::Create(aiNode * pAINode, CAssimpBone * pParent, _uint iIndex)
{
	CAssimpBone*	pInstance = new CAssimpBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent, iIndex)))
	{
		MSG_BOX("Failed to Created CAssimpBone");
		Safe_Delete(pInstance);
	}
	return pInstance;
}

ParsingData* CAssimpBone::Save_Data(HANDLE handle, ParsingData* data)
{
	ModelParsingData* myData = (ModelParsingData*)data;
	BoneParsingData boneData;

	strcpy(boneData.szName, m_szName);
	memcpy(&boneData.TransformationMatrix, &m_TransformationMatrix, sizeof(_float4x4));
	memcpy(&boneData.CombinedTransformationMatrix, &m_CombinedTransformationMatrix, sizeof(_float4x4));
	memcpy(&boneData.OffsetMatrix, &m_OffsetMatrix, sizeof(_float4x4));
	boneData.iParentIndex = m_iParentIndex;
	boneData.iIndex = m_iIndex;

	myData->BoneDatas.push_back(boneData);
	return nullptr;
}

