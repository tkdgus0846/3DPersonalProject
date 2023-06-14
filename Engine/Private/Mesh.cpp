#include "..\Public\Mesh.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

void CMesh::Get_Matrices(CModel::BONES Bones, _float4x4* pMatrices, _fmatrix PivotMatrix)
{
	_uint		iIndex = 0;

	for (auto iBoneIndex : m_BoneIndices)
	{
		// pMatrices[iIndex++] = Bones[iBoneIndex]->Get_OffsetMatrix() * Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
		XMStoreFloat4x4(&pMatrices[iIndex++],
			XMLoadFloat4x4(&Bones[iBoneIndex]->Get_OffsetMatrix()) *
			XMLoadFloat4x4(&Bones[iBoneIndex]->Get_CombinedTransformationMatrix()) *
			PivotMatrix);
	}
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, const CModel::BONES& Bones, const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iNumVertexBuffers = { 1 };

	m_iNumVertices = { pAIMesh->mNumVertices };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { pAIMesh->mNumFaces * 3 };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eType)
		hr = Ready_VertexBuffer_NonAnim(pAIMesh, PivotMatrix);
	else
		hr = Ready_VertexBuffer_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;


#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumFaces = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumFaces++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumFaces++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumFaces++] = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iStride = { sizeof(VTXMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

	return S_OK;

}

HRESULT CMesh::Ready_VertexBuffer_Anim(const aiMesh* pAIMesh, const CModel::BONES& Bones)
{
	m_iStride = { sizeof(VTXANIMMESH) };

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.CPUAccessFlags = { 0 };
	m_BufferDesc.MiscFlags = { 0 };

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexCoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_iNumBones = pAIMesh->mNumBones;

	/* pAIMesh->mNumBones : 이 메시는 몇개의 뼈의 정보를 사용하는가. */
	for (size_t i = 0; i < pAIMesh->mNumBones; ++i)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_uint		iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue)
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

			if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.x)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndices.x = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeights.x = AIVertexWeight.mWeight;
			}

			else if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndices.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeights.y = AIVertexWeight.mWeight;
			}

			else if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndices.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeights.z = AIVertexWeight.mWeight;
			}

			else if (0.0f == pVertices[AIVertexWeight.mVertexId].vBlendWeights.w)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndices.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeights.w = AIVertexWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint	iBoneIndex = 0;

		// 모델이 들고 있는 같은 이름을 가진 뼈를 찾느다. 
		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue)
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

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);




	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const CModel::BONES& Bones, const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, Bones, pAIMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Created CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CMesh::Free()
{
	__super::Free();
}
