#include "..\Public\Bone.h"

CBone::CBone()
{
}

CBone::CBone(const CBone & rhs)
	: m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_CombinedTransformationMatrix(rhs.m_CombinedTransformationMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_iParentIndex(rhs.m_iParentIndex)
	, m_iIndex(rhs.m_iIndex)
{
	strcpy_s(m_szName, rhs.m_szName);
}


HRESULT CBone::Initialize(aiNode * pAINode, CBone * pParent, _uint iIndex)
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

void CBone::Invalidate_CombinedTransformationMatrix(const CModel::BONES& Bones)
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

CBone * CBone::Create(aiNode * pAINode, CBone * pParent, _uint iIndex)
{
	CBone*	pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent, iIndex)))
	{
		MSG_BOX("Failed to Created CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);	
}

void CBone::Free()
{

}
