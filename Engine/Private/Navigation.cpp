#include "..\Public\Navigation.h"
#include "Cell.h"

#ifdef _DEBUG
#include "Shader.h"
#include "PipeLine.h"
#endif

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	, m_TriangleList(rhs.m_TriangleList)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_BoundingSpheres(rhs.m_BoundingSpheres)
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
	Safe_AddRef(m_pInputLayout);
#endif

}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationDataFiles)
{
	_ulong		dwByte = { 0 };

	HANDLE		hFile = CreateFile(pNavigationDataFiles, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[CCell::POINT_END];

	_int index = 0;
	while(true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		Add_Cell(vPoints);

		_float3 point1, point2, point3;
		XMStoreFloat3(&point1, m_Cells[index]->Get_Point(CCell::POINT_A));
		XMStoreFloat3(&point2, m_Cells[index]->Get_Point(CCell::POINT_B));
		XMStoreFloat3(&point3, m_Cells[index]->Get_Point(CCell::POINT_C));

		m_TriangleList.push_back(Triangle(point1, point2, point3));

		index++;
	}

	CloseHandle(hFile);	

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode;
	size_t		iShaderByteCodeLength;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memmove(&m_NaviDesc, pArg, sizeof(NAVIGATIONDESC));

	return S_OK;
}

_bool CNavigation::is_Move(_fvector vPosition, _int* iNeighbor)
{
	_int		iNeighborIndex = -1;

	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->is_In(vPosition, &iNeighborIndex, iNeighbor))
	{
		return true;
	}
	else /* 밖으로 나갔어. */
	{
		if (-1 != iNeighborIndex) /* 이웃이  있다라면. */
		{
			while (true)
			{
				if (-1 == iNeighborIndex) // 이웃이 없다라면
					return false;

				if (true == m_Cells[iNeighborIndex]->is_In(vPosition, &iNeighborIndex, iNeighbor)) // 이웃이 있었고 다음 안에 들어온다면
					break;
			}
			m_NaviDesc.iCurrentIndex = iNeighborIndex;

			return true;
		}
		else
			return false;
	}

	return false;
}

void CNavigation::Add_Cell(_float3* vPoints)
{
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());

	if (nullptr == pCell)
		return;

	m_Cells.push_back(pCell);

#ifdef _DEBUG

	// 기존에 있던 셀들을 순회하면서 다시 만들어줘야한다.

	vector<BoundingSphere> cellSphereVec = pCell->Get_All_Sphere();
	for (int i = 0; i < 3; i++)
		m_BoundingSpheres.push_back(cellSphereVec[i]);
#endif
}

vector<Triangle>* CNavigation::Get_TriangleList()
{
	return &m_TriangleList;
}

_vector CNavigation::Get_PlaneNormal(_uint iIndex)
{
	return m_Cells[iIndex]->Get_PlaneNormal();
}

_int CNavigation::SlidingVector(_uint& index, _fvector pos, _fvector dir, _float3& slidingVector, map<_uint, _uint>& history)
{
	return m_Cells[index]->SlidingVector(index, pos, dir, slidingVector, history);
}

_vector CNavigation::Get_Normal(_int neighbor)
{
	return m_Cells[m_NaviDesc.iCurrentIndex]->Get_Normal(neighbor);
}

#ifdef _DEBUG

HRESULT CNavigation::Render_Navigation()
{
	if (m_bRender == false) return S_OK;

	if (nullptr == m_pShader)
		return E_FAIL;
	if (nullptr == m_pBatch)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());


	CPipeLine*		pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	//m_pBounding->Render(m_pBatch);
		
	Safe_Release(pPipeLine);

	_float4		vColor = -1 == m_NaviDesc.iCurrentIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);

	if (-1 == m_NaviDesc.iCurrentIndex)
	{

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		for (auto& pCell : m_Cells)
		{
			pCell->Render_BoundingSpheres(m_pBatch);
		}

		for (auto& pCell : m_Cells)
		{
			CCell::OPTION option = (CCell::OPTION)pCell->Get_Option();
			
			switch(option)		
			{
				case CCell::FALL:
					vColor = { 1.f, 0.f, 1.f, 0.f };
					break;
				case CCell::NOPASS:
					vColor = { 1.f, 0.f, 0.f, 0.f };
					break;
				case CCell::NORMAL:
					vColor = { 0.f, 1.f, 0.f, 0.f };
					break;
			}	

			if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
				return E_FAIL;

			m_pShader->Begin(0);

			pCell->Render();
				
		}
	}
	else
	{
		WorldMatrix._42 = 0.1f;
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		vColor = _float4(0.f, 0.f, 1.f, 1.f);

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
			return E_FAIL;
		m_pShader->Begin(0);

		m_Cells[m_NaviDesc.iCurrentIndex]->Render();
	}

	m_pBatch->End();

	return S_OK;
}

vector<BoundingSphere>* CNavigation::Get_All_Sphere()
{
	return &m_BoundingSpheres;
}

_float3 CNavigation::Picking_Cell(_uint index)
{
	_uint cellIndex, pointIndex;

	ConvertIndex(index, &cellIndex, &pointIndex);
	_float3 pos = m_Cells[cellIndex]->Picking_Point(pointIndex);

	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		if (i == cellIndex) continue;

		for (_uint j = 0;  j < 3; j++)
		{
			if (XMVector3Equal(XMLoadFloat3(&pos), m_Cells[i]->Get_Point((CCell::POINT)j)))
			{
				m_Cells[i]->Picking_Point(j);
			}
		}
		
	}

	return pos;
}

void CNavigation::Reset_Selected_AllCells()
{
	for (CCell* cell : m_Cells)
	{
		cell->Reset_Selected();
	}
}

void CNavigation::Translate(_uint index, _fvector trans)
{
	_uint cellIndex, pointIndex;

	ConvertIndex(index, &cellIndex, &pointIndex);
	_vector pos = m_Cells[cellIndex]->Get_Point((CCell::POINT)pointIndex);
	m_Cells[cellIndex]->Translate(pointIndex, trans);
	XMStoreFloat3(&m_BoundingSpheres[index].Center, trans);

	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		if (i == cellIndex) continue;

		for (_uint j = 0; j < 3; j++)
		{
			if (XMVector3Equal(pos, m_Cells[i]->Get_Point((CCell::POINT)j)))
			{
				m_Cells[i]->Translate(j, trans);
				XMStoreFloat3(&m_BoundingSpheres[i * 3 + j].Center, trans);
			}
		}
	}
}

void CNavigation::ConvertIndex(_uint AllIndex, _uint* CellIndex, _uint* PointIndex)
{
	// 전체의 인덱스를 받아와서 셀 인덱스로 바꾸는작업.
	_uint resultIndex = AllIndex;

	if (CellIndex == nullptr) return;
	// 4 => 1 7 => 2
	*CellIndex = AllIndex / 3;

	if (PointIndex == nullptr) return;
	// 4 % 3 = 1
	*PointIndex = AllIndex % 3;
	
}

void CNavigation::Reset_BoundingSpheres()
{
	m_BoundingSpheres.clear();

	for (CCell* cell : m_Cells)
	{
		vector<BoundingSphere> vec = cell->Get_All_Sphere();

		for (BoundingSphere& sphere : vec)
		{
			m_BoundingSpheres.push_back(sphere);
		}
	}
}

#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if(true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
			}
		}
	}
	
	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationDataFiles)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFiles)))
	{
		MSG_BOX("Failed to Created CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pShader);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);

	}
	Safe_Release(m_pInputLayout);
#endif

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}

void CNavigation::Save_Navigation(const wstring& name)
{
	wstring pFilePath = L"../../NavMeshes/" + name + L".dat";
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

	_float3		vPoints[CCell::POINT_END];
	for (auto pCell : m_Cells)
	{
		for (int i = 0; i < CCell::POINT_END; i++)
		{
			_vector point = pCell->Get_Point((CCell::POINT)i);
			_float3 savePos;
			XMStoreFloat3(&savePos, point);

			WriteFile(hFile, &savePos, sizeof savePos, &dwByte, nullptr);
		}
	}
}
