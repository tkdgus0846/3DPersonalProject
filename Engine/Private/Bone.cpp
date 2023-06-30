#include "..\Public\Bone.h"

CBone::CBone()
{
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
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


HRESULT CBone::Initialize(ParsingData* pData)
{
	BoneParsingData* data = (BoneParsingData*)pData;


	strcpy(m_szName, data->szName);
	
	memcpy(&m_TransformationMatrix, &data->TransformationMatrix, sizeof(_float4x4));
	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	memcpy(&m_OffsetMatrix, &data->OffsetMatrix, sizeof(_float4x4));
	//memcpy(&m_CombinedTransformationMatrix, &data->CombinedTransformationMatrix, sizeof(_float4x4));
	
	m_iParentIndex = data->iParentIndex;
	m_iIndex = data->iIndex;

	string str = m_szName;

	if (str.find("Pelvis") != std::string::npos)
		int i = 0;

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

CBone * CBone::Create(ParsingData* pData)
{
	CBone*	pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pData)))
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

ParsingData* CBone::Load_Data(HANDLE handle, ParsingData* data)
{
	DWORD dwByte = 0;
	char str[MAX_PATH];
	ModelParsingData* myData = (ModelParsingData*)data;
	BoneParsingData boneData;
	
	ReadFile(handle, &str, sizeof(str), &dwByte, nullptr);

	strcpy(boneData.szName, str);

	ReadFile(handle, &boneData.TransformationMatrix, sizeof(boneData.TransformationMatrix), &dwByte, nullptr);
	ReadFile(handle, &boneData.CombinedTransformationMatrix, sizeof(boneData.CombinedTransformationMatrix), &dwByte, nullptr);
	ReadFile(handle, &boneData.OffsetMatrix, sizeof(boneData.OffsetMatrix), &dwByte, nullptr);
	ReadFile(handle, &boneData.iParentIndex, sizeof(boneData.iParentIndex), &dwByte, nullptr);
	ReadFile(handle, &boneData.iIndex, sizeof(boneData.iIndex), &dwByte, nullptr);

	myData->BoneDatas.push_back(boneData);
	return S_OK;
}
