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

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, const CModel::BONES& Bones, ParsingData* pData, _fmatrix PivotMatrix)
{
	MeshParsingData* data = (MeshParsingData*)pData;
	
	m_iMaterialIndex = data->iMaterialIndex;
	strcpy_s(m_szName, data->szName);
	m_iNumVertexBuffers = { 1 };

	m_iNumVertices = { data->iNumVertices };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { data->iNumIndices };
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eType)
		hr = Ready_VertexBuffer_NonAnim(data, PivotMatrix);
	else
		hr = Ready_VertexBuffer_Anim(data, Bones);

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

	for (size_t i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = data->Indices[i];
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

HRESULT CMesh::Ready_VertexBuffer_NonAnim(MeshParsingData* pData, _fmatrix PivotMatrix)
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
		memcpy(&pVertices[i].vPosition, &pData->Vertices[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pData->Vertices[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexCoord, &pData->Vertices[i].vTexCoord, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pData->Vertices[i].vTangent, sizeof(_float3));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(MeshParsingData* pData, const CModel::BONES& Bones)
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
		memcpy(&pVertices[i].vPosition, &pData->Vertices[i].vPosition, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pData->Vertices[i].vNormal, sizeof(_float3));
		memcpy(&pVertices[i].vTexCoord, &pData->Vertices[i].vTexCoord, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pData->Vertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndices, &pData->Vertices[i].vBlendIndices, sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeights, &pData->Vertices[i].vBlendWeights, sizeof(XMFLOAT4));
	}

	m_iNumBones = pData->iNumBones;

	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		m_BoneIndices.push_back(pData->BoneIndices[i]);
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

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const CModel::BONES& Bones, ParsingData* pData, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, Bones, pData, PivotMatrix)))
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

ParsingData* CMesh::Load_Data(HANDLE handle, ParsingData* data)
{
	DWORD dwByte = 0;
	char str[MAX_PATH];
	ModelParsingData* myData = (ModelParsingData*)data;
	MeshParsingData meshData;

	ReadFile(handle, &str, sizeof(str), &dwByte, nullptr);
	strcpy(meshData.szName, str);

	ReadFile(handle, &meshData.iMaterialIndex, sizeof(meshData.iMaterialIndex), &dwByte, nullptr);
	ReadFile(handle, &meshData.iNumVertices, sizeof(meshData.iNumVertices), &dwByte, nullptr);
	ReadFile(handle, &meshData.iNumIndices, sizeof(meshData.iNumIndices), &dwByte, nullptr);

	for (int i = 0; i < meshData.iNumIndices; i++)
	{
		_ulong Index;
		ReadFile(handle, &Index, sizeof(Index), &dwByte, nullptr);
		meshData.Indices.push_back(Index);
	}

	for (int i = 0; i < meshData.iNumVertices; i++)
	{
		VTXANIMMESH Vertex;
		ReadFile(handle, &Vertex, sizeof(Vertex), &dwByte, nullptr);
		meshData.Vertices.push_back(Vertex);
	}

	ReadFile(handle, &meshData.iNumBones, sizeof(meshData.iNumBones), &dwByte, nullptr);

	for (int i = 0; i < meshData.iNumBones; i++)
	{
		_uint BoneIndex;
		ReadFile(handle, &BoneIndex, sizeof(BoneIndex), &dwByte, nullptr);
		meshData.BoneIndices.push_back(BoneIndex);
	}

	myData->MeshDatas.push_back(meshData);
	return nullptr;
}
