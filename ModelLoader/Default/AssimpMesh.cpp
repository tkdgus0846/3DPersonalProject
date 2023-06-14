#include "stdafx.h"
#include "AssimpMesh.h"
#include "AssimpBone.h"

CAssimpMesh::CAssimpMesh()
{
}

CAssimpMesh::~CAssimpMesh()
{
}

HRESULT CAssimpMesh::Initialize_Prototype(const CAssimpModel::BONES& Bones, const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iNumVertices = { pAIMesh->mNumVertices };
	m_iNumIndices = { pAIMesh->mNumFaces * 3 };

	HRESULT		hr = 0;

	hr = Ready_VertexBuffer_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;

	m_Indices.resize(m_iNumIndices);

	_uint		iNumFaces = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		m_Indices[iNumFaces++] = pAIMesh->mFaces[i].mIndices[0];
		m_Indices[iNumFaces++] = pAIMesh->mFaces[i].mIndices[1];
		m_Indices[iNumFaces++] = pAIMesh->mFaces[i].mIndices[2];
	}

	return S_OK;
}

HRESULT CAssimpMesh::Ready_VertexBuffer_Anim(const aiMesh* pAIMesh, const CAssimpModel::BONES& Bones)
{
	m_Vertices.resize(m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_Vertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_Vertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_Vertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_Vertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_iNumBones = pAIMesh->mNumBones;

	/* pAIMesh->mNumBones : 이 메시는 몇개의 뼈의 정보를 사용하는가. */
	for (size_t i = 0; i < pAIMesh->mNumBones; ++i)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_uint		iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CAssimpBone* pValue)
			{
				if (0 != strcmp(pAIBone->mName.data, pValue->Get_Name()))
				{
					++iBoneIndex;
					return false;
				}
				else
					return true;
			});

		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		Bones[iBoneIndex]->Set_OffsetMatrix(OffsetMatrix);

		m_BoneIndices.push_back(iBoneIndex);

		/*pAIBone->mNumWeights : 이뼈는 몇개 정점에게 영향을 주니.  */
		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			/* AIVertexWeight : j번째 정점에게 영향을 주는정보. */
			aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];

			if (0.0f == m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.x)
			{
				m_Vertices[AIVertexWeight.mVertexId].vBlendIndices.x = i;
				m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.x = AIVertexWeight.mWeight;
			}

			else if (0.0f == m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.y)
			{
				m_Vertices[AIVertexWeight.mVertexId].vBlendIndices.y = i;
				m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.y = AIVertexWeight.mWeight;
			}

			else if (0.0f == m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.z)
			{
				m_Vertices[AIVertexWeight.mVertexId].vBlendIndices.z = i;
				m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.z = AIVertexWeight.mWeight;
			}

			else if (0.0f == m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.w)
			{
				m_Vertices[AIVertexWeight.mVertexId].vBlendIndices.w = i;
				m_Vertices[AIVertexWeight.mVertexId].vBlendWeights.w = AIVertexWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint	iBoneIndex = 0;

		// 모델이 들고 있는 같은 이름을 가진 뼈를 찾느다. 
		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CAssimpBone* pValue)
			{
				if (0 != strcmp(m_szName, pValue->Get_Name()))
				{
					++iBoneIndex;
					return false;
				}
				else
					return true;
			});


		m_BoneIndices.push_back(iBoneIndex);
	}
	return S_OK;
}

CAssimpMesh* CAssimpMesh::Create(const CAssimpModel::BONES& Bones, const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	CAssimpMesh* pInstance = new CAssimpMesh();

	if (FAILED(pInstance->Initialize_Prototype(Bones, pAIMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CAssimpMesh");
		Safe_Delete(pInstance);
	}
	return pInstance;
}

HRESULT CAssimpMesh::Save_Data(ParsingData* data)
{
	ModelParsingData* myData = (ModelParsingData*)&data;
	MeshParsingData meshData;
	
	strcpy(meshData.szName, m_szName);
	meshData.iMaterialIndex = m_iMaterialIndex;
	meshData.iNumVertices = m_iNumVertices;
	meshData.Indices = m_Indices;
	meshData.Vertices = m_Vertices;
	meshData.iNumBones = m_iNumBones;
	meshData.BoneIndices = m_BoneIndices;

	myData->MeshDatas.push_back(meshData);
	return S_OK;
}


