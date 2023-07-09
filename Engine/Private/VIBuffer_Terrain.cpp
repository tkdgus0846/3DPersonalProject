#include "..\Public\VIBuffer_Terrain.h"
#include "QuadTree.h"
#include "Frustum.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX{ rhs.m_iNumVerticesX }
	, m_iNumVerticesZ{ rhs.m_iNumVerticesZ }
	, m_fInterval{ rhs.m_fInterval }
	, m_VerticesPos{ rhs.m_VerticesPos}
	, m_pIndices(rhs.m_pIndices)
	, m_pQuadTree(rhs.m_pQuadTree)
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMap, TYPE eType)
{
	_ulong			dwByte = { 0 };
	HANDLE			hFile = CreateFile(pHeightMap, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	BITMAPFILEHEADER	fh;
	BITMAPINFOHEADER	ih;

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);	

	m_iNumVerticesX = { (_uint)ih.biWidth };
	m_iNumVerticesZ = { (_uint)ih.biHeight };

	_ulong*		pPixel = new _ulong[m_iNumVerticesX * m_iNumVerticesZ];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	CloseHandle(hFile);


	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { m_iNumVerticesX * m_iNumVerticesZ };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3 };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST } ;


#pragma region VERTEX_BUFFER
	
	
	VTXPOSNORTEX*		pVertices = new VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
						
			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(j * m_fInterval, (pPixel[iIndex] & 0x000000ff) / 10.0f, i * m_fInterval);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexCoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}	

	
#pragma endregion


#pragma region INDEX_BUFFER
	

	_ulong*		pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			_uint indexArr[3];
			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[1];
			indexArr[2] = iIndices[2];

			m_VerticesPos.push_back(Triangle(pVertices[iIndices[0]].vPosition, pVertices[iIndices[1]].vPosition, pVertices[iIndices[2]].vPosition, iIndices[0], iIndices[1], iIndices[2]));

			Compute_Normal(indexArr, pVertices);
			

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[2];
			indexArr[2] = iIndices[3];

			m_VerticesPos.push_back(Triangle(pVertices[iIndices[0]].vPosition, pVertices[iIndices[2]].vPosition, pVertices[iIndices[3]].vPosition, iIndices[0], iIndices[2], iIndices[3]));

			Compute_Normal(indexArr, pVertices);
		}
	}

	for (_ulong i = 0; i < m_iNumVertices; ++i)
	{
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal));
		XMStoreFloat3(&pVertices[i].vNormal, vNormal);
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };

	if (eType == TYPE::STATIC)
		m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	else
		m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };

	if (eType == TYPE::STATIC)
		m_BufferDesc.CPUAccessFlags = { 0 };
	else
		m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };

	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixel);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1,
		m_iNumVerticesX - 1,
		0);


	if (FAILED(m_pQuadTree->Make_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _uint& sizeX, const _uint& sizeZ, const _float& fInterval, TYPE eType)
{

	m_iNumVerticesX = sizeX;
	m_iNumVerticesZ = sizeZ;
	m_fInterval = fInterval;

	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { m_iNumVerticesX * m_iNumVerticesZ };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3 };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };


#pragma region VERTEX_BUFFER


	VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(j * m_fInterval, 0.f , i * m_fInterval);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexCoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}


#pragma endregion


#pragma region INDEX_BUFFER


	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			_uint indexArr[3];
			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[1];
			indexArr[2] = iIndices[2];

			m_VerticesPos.push_back(Triangle(pVertices[iIndices[0]].vPosition, pVertices[iIndices[1]].vPosition, pVertices[iIndices[2]].vPosition, iIndices[0], iIndices[1], iIndices[2]));

			Compute_Normal(indexArr, pVertices);


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[2];
			indexArr[2] = iIndices[3];

			m_VerticesPos.push_back(Triangle(pVertices[iIndices[0]].vPosition, pVertices[iIndices[2]].vPosition, pVertices[iIndices[3]].vPosition, iIndices[0], iIndices[2], iIndices[3]));

			Compute_Normal(indexArr, pVertices);
		}
	}

	for (_ulong i = 0; i < m_iNumVertices; ++i)
	{
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal));
		XMStoreFloat3(&pVertices[i].vNormal, vNormal);
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	if (eType == TYPE::STATIC)
		m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	else
		m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };

	if (eType == TYPE::STATIC)
		m_BufferDesc.CPUAccessFlags = { 0 };
	else
		m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };
	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1,
		m_iNumVerticesX - 1,
		0);


	if (FAILED(m_pQuadTree->Make_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& pCustomHeightMap, TYPE eType)
{
	_ulong			dwByte = { 0 };
	HANDLE			hFile = CreateFile(pCustomHeightMap.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	ReadFile(hFile, &m_iNumVerticesX, sizeof m_iNumVerticesX, &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVerticesZ, sizeof m_iNumVerticesZ, &dwByte, nullptr);
	ReadFile(hFile, &m_fInterval, sizeof m_fInterval, &dwByte, nullptr);

	_float* pPixel = new _float[m_iNumVerticesX * m_iNumVerticesZ];
	ZeroMemory(pPixel, sizeof(_float) * m_iNumVerticesX * m_iNumVerticesZ);

	ReadFile(hFile, pPixel, sizeof(_float) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	CloseHandle(hFile);


	m_iNumVertexBuffers = { 1 };
	m_iStride = { sizeof(VTXPOSNORTEX) };
	m_iNumVertices = { m_iNumVerticesX * m_iNumVerticesZ };
	m_iIndexStride = { sizeof(_ulong) };
	m_iNumIndices = { (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3 };
	m_eFormat = { DXGI_FORMAT_R32_UINT };
	m_eTopology = { D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };


#pragma region VERTEX_BUFFER


	VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(j * m_fInterval, pPixel[iIndex], i * m_fInterval);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexCoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}


#pragma endregion


#pragma region INDEX_BUFFER


	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			_uint indexArr[3];
			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[1];
			indexArr[2] = iIndices[2];

			m_VerticesPos.push_back(Triangle(pVertices[iIndices[0]].vPosition, pVertices[iIndices[1]].vPosition, pVertices[iIndices[2]].vPosition, iIndices[0], iIndices[1], iIndices[2]));

			Compute_Normal(indexArr, pVertices);


			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[2];
			indexArr[2] = iIndices[3];

			m_VerticesPos.push_back(Triangle(pVertices[iIndices[0]].vPosition, pVertices[iIndices[2]].vPosition, pVertices[iIndices[3]].vPosition, iIndices[0], iIndices[2], iIndices[3]));

			Compute_Normal(indexArr, pVertices);
		}
	}

	for (_ulong i = 0; i < m_iNumVertices; ++i)
	{
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal));
		XMStoreFloat3(&pVertices[i].vNormal, vNormal);
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iStride * m_iNumVertices };

	if (eType == TYPE::STATIC)
		m_BufferDesc.Usage = { D3D11_USAGE_DEFAULT };
	else
		m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };

	if (eType == TYPE::STATIC)
		m_BufferDesc.CPUAccessFlags = { 0 };
	else
		m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };

	m_BufferDesc.BindFlags = { D3D11_BIND_VERTEX_BUFFER };
	m_BufferDesc.StructureByteStride = { m_iStride };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixel);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = { m_iIndexStride * m_iNumIndices };
	m_BufferDesc.Usage = { D3D11_USAGE_DYNAMIC };
	m_BufferDesc.BindFlags = { D3D11_BIND_INDEX_BUFFER };
	m_BufferDesc.StructureByteStride = { 0 };
	m_BufferDesc.CPUAccessFlags = { D3D11_CPU_ACCESS_WRITE };
	m_BufferDesc.MiscFlags = { 0 };

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1,
		m_iNumVerticesX - 1,
		0);


	if (FAILED(m_pQuadTree->Make_Neighbors()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix, _bool bLOD)
{
	CFrustum* pFrustum = CFrustum::GetInstance();
	Safe_AddRef(pFrustum);

	pFrustum->Transform_ToLocalSpace(WorldMatrix);

	_uint		iNumIndices = { 0 };

	
	m_pQuadTree->Culling(pFrustum, m_pVerticesPos, m_pIndices, &iNumIndices, bLOD);

	//for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	//{
	//	for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
	//	{
	//		_uint		iIndex = i * m_iNumVerticesX + j;

	//		_uint		iIndices[4] = {
	//			iIndex + m_iNumVerticesX,
	//			iIndex + m_iNumVerticesX + 1,
	//			iIndex + 1,
	//			iIndex
	//		};

	//		_bool		isIn[4] = {
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), 0.0f),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), 0.0f),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), 0.0f),
	//			pFrustum->isIn_LocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[3]]), 0.0f),
	//		};

	//		/* 오른쪽 위 삼각형을 구성하는 정점 중, 최소 하나라도 절두체 안에 있다. */
	//		if (true == isIn[0] ||
	//			true == isIn[1] ||
	//			true == isIn[2])
	//		{
	//			m_pIndices[iNumIndices++] = iIndices[0];
	//			m_pIndices[iNumIndices++] = iIndices[1];
	//			m_pIndices[iNumIndices++] = iIndices[2];
	//		}

	//		/* 왼쪽 아래 삼각형을 구성하는 정점 중, 최소 하나라도 절두체 안에 있다. */
	//		if (true == isIn[0] ||
	//			true == isIn[2] ||
	//			true == isIn[3])
	//		{
	//			m_pIndices[iNumIndices++] = iIndices[0];
	//			m_pIndices[iNumIndices++] = iIndices[2];
	//			m_pIndices[iNumIndices++] = iIndices[3];
	//		}
	//	}
	//}

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_pIndices, sizeof(_ulong) * iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;

	Safe_Release(pFrustum);
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::Picking_Height_Cos(_uint index, _float radius, _float power, _fvector centerPos)
{
	unordered_set<_uint> visited;
	queue<_uint> indexQueue;
	vector<pair<_uint, _float>> resultVec;
	_int dx[4] = { (_int)m_iNumVerticesX, 1, -(_int)m_iNumVerticesX, -1 };

	// 현재 내삼각형의 정점 세개에 대해서 검사한다.

	_uint index1 = m_VerticesPos[index].iIndex1;
	_uint index2 = m_VerticesPos[index].iIndex2;
	_uint index3 = m_VerticesPos[index].iIndex3;

	indexQueue.push(index1);
	indexQueue.push(index2);
	indexQueue.push(index3);

	visited.insert(index1);
	visited.insert(index2);
	visited.insert(index3);

	resultVec.push_back({ index1, Compute_Height(Compute_Pos(index1), radius, power, centerPos) });
	resultVec.push_back({ index2, Compute_Height(Compute_Pos(index2), radius, power, centerPos) });
	resultVec.push_back({ index3, Compute_Height(Compute_Pos(index3), radius, power, centerPos) });

	while (!indexQueue.empty())
	{
		_int curIndex = indexQueue.front();
		indexQueue.pop();

		for (int i = 0; i < 4; i++)
		{
			_int willIndex = curIndex + dx[i];
			_vector pos = Compute_Pos(willIndex);
			_float distance = Compute_Distance(pos, centerPos);;

			// 거리계산 (willIndex 의 정점과 CenterPos 사이의 거리);
			if (willIndex < 0 || willIndex >= m_iNumVertices || visited.find(willIndex) != visited.end() || distance > radius) continue;

			_float height = Compute_Height(pos, radius, power, centerPos);
			
			indexQueue.push(willIndex);
			resultVec.push_back({ willIndex, height });
			visited.insert(willIndex);
		}	
	}

	// 동적 버텍스 버퍼 매핑
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);

	// mappedResource.pData를 통해 동적 버텍스 버퍼의 데이터에 접근할 수 있음
	VTXPOSNORTEX* vertices = reinterpret_cast<VTXPOSNORTEX*>(mappedResource.pData);

	for (auto& item : resultVec)
	{
		m_pVerticesPos[item.first].y += item.second;
		vertices[item.first].vPosition.y += item.second;
	}

	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Terrain::Backing_Normal_All()
{
	m_VerticesPos.clear();
	// 동적 버텍스 버퍼 매핑
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);

	// mappedResource.pData를 통해 동적 버텍스 버퍼의 데이터에 접근할 수 있음
	VTXPOSNORTEX* vertices = reinterpret_cast<VTXPOSNORTEX*>(mappedResource.pData);

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_uint indexArr[3];
			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[1];
			indexArr[2] = iIndices[2];

			m_VerticesPos.push_back(Triangle(vertices[iIndices[0]].vPosition, vertices[iIndices[1]].vPosition, vertices[iIndices[2]].vPosition, iIndices[0], iIndices[1], iIndices[2]));

			Compute_Normal(indexArr, vertices);

			indexArr[0] = iIndices[0];
			indexArr[1] = iIndices[2];
			indexArr[2] = iIndices[3];

			m_VerticesPos.push_back(Triangle(vertices[iIndices[0]].vPosition, vertices[iIndices[2]].vPosition, vertices[iIndices[3]].vPosition, iIndices[0], iIndices[2], iIndices[3]));

			Compute_Normal(indexArr, vertices);
		}
	}
	
	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Terrain::Compute_Normal(const _uint* verticesIndex, VTXPOSNORTEX* pVertices)
{
	_vector vDest = XMLoadFloat3(&pVertices[verticesIndex[1]].vPosition) - XMLoadFloat3(&pVertices[verticesIndex[0]].vPosition);
	_vector vSrc = XMLoadFloat3(&pVertices[verticesIndex[2]].vPosition) - XMLoadFloat3(&pVertices[verticesIndex[1]].vPosition);

	_vector vNormal = XMVector3Cross(vDest, vSrc);

	XMStoreFloat3(&pVertices[verticesIndex[0]].vNormal, XMLoadFloat3(&pVertices[verticesIndex[0]].vNormal) + vNormal);
	XMStoreFloat3(&pVertices[verticesIndex[1]].vNormal, XMLoadFloat3(&pVertices[verticesIndex[1]].vNormal) + vNormal);
	XMStoreFloat3(&pVertices[verticesIndex[2]].vNormal, XMLoadFloat3(&pVertices[verticesIndex[2]].vNormal) + vNormal);
}

_float CVIBuffer_Terrain::Compute_Height(_fvector originPos, _float radius, _float power, _fvector centerPos)
{
	// x z 만 비교 하자. 
	_float distance = Compute_Distance(originPos, centerPos);

	// 0~1 사이의 값으로 나올것임.
	_float ratio = distance / radius;

	// 0~파이/2 사이의 값으로 나올것임.
	ratio = ratio * XMConvertToRadians(90.f);

	return cosf(ratio) * power;
}

_vector CVIBuffer_Terrain::Compute_Pos(_uint index)
{
	_vector resultVec = { 0.f,0.f,0.f,1.f };
	_uint vertical = index / m_iNumVerticesX * m_fInterval;
	_uint horizontal = index % m_iNumVerticesX * m_fInterval;

	resultVec.m128_f32[0] = horizontal;
	resultVec.m128_f32[2] = vertical;

	return resultVec;
}

_float CVIBuffer_Terrain::Compute_Distance(_fvector aPos, _fvector bPos)
{
	_vector a = XMVectorSet(aPos.m128_f32[0], aPos.m128_f32[2], 0.f, 0.f);
	_vector b = XMVectorSet(bPos.m128_f32[0], bPos.m128_f32[2], 0.f, 0.f);

	_float distance = DirectX::XMVector2Length(DirectX::XMVectorSubtract(a, b)).m128_f32[0];
	return distance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pHeightMap, TYPE eType)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMap, eType)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pCustomHeightMap, TYPE eType)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pCustomHeightMap, eType)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint& sizeX, const _uint& sizeZ, const _float& fInterval, TYPE eType)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(sizeX,sizeZ,fInterval,eType)))
	{
		MSG_BOX("Failed to Created CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pIndices);

	Safe_Release(m_pQuadTree);

}

void CVIBuffer_Terrain::Save_TerrainBuffer(const wstring& name)
{
	wstring pFilePath = L"../../Terrains/" + name + L".dat";
	HANDLE		hFile = CreateFile
	(pFilePath.c_str(),
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		//MSG_BOX("Failed to Save File");
		return;
	}

	DWORD dwByte = 0;

	WriteFile(hFile, &m_iNumVerticesX, sizeof m_iNumVerticesX, &dwByte, nullptr);
	WriteFile(hFile, &m_iNumVerticesZ, sizeof m_iNumVerticesZ, &dwByte, nullptr);
	WriteFile(hFile, &m_fInterval, sizeof m_fInterval, &dwByte, nullptr);

	// 동적 버텍스 버퍼 매핑
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);

	// mappedResource.pData를 통해 동적 버텍스 버퍼의 데이터에 접근할 수 있음
	VTXPOSNORTEX* vertices = reinterpret_cast<VTXPOSNORTEX*>(mappedResource.pData);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_int index = i * m_iNumVerticesX + j;

			WriteFile(hFile, &vertices[index].vPosition.y, sizeof(_float), &dwByte, nullptr);
		}
	}

	m_pContext->Unmap(m_pVB, 0);

	CloseHandle(hFile);


}


