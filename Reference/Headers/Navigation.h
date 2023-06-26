#pragma once

#include "Component.h"

/* 네비게이션을 구성하기위한 CCell(삼각형)들을 들고 있는다.  */

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigation
	{
		tagNavigation()
			: iCurrentIndex(-1) { }
		explicit tagNavigation(_int _iCurrentIndex)
			: iCurrentIndex{ _iCurrentIndex } {	}
		_int	iCurrentIndex = { -1 };
	}NAVIGATIONDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFiles);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool is_Move(_fvector vPosition, _int* iNeighbor = nullptr);
	void Add_Cell(_float3* vPoints);
	vector<Triangle>* Get_TriangleList();
	_int Get_CurrentIndex() const { return m_NaviDesc.iCurrentIndex; }
	_vector Get_PlaneNormal(_uint iIndex); 

	// 현재 이동중인 놈의 위치와 방향을 넣어주면 슬라이딩 벡터를 뽑아준다.
	_int SlidingVector(_uint& index, _fvector pos, _fvector dir, _float3& slidingVector, map<_uint, _uint>& history);
	_vector Get_Normal(_int neighbor);
	

#ifdef _DEBUG
public:
	HRESULT Render_Navigation();
	
	vector<BoundingSphere>* Get_All_Sphere();

	// 광선을 쏴서 맞췄을때. 어떤 셀에있는 Picking_Point를 불러줄건가?
	_float3 Picking_Cell(_uint index);

	void Reset_Selected_AllCells();

	void Translate(_uint index, _fvector trans);
	void Set_Render(_bool bRender) { m_bRender = bRender; }
	

private:
	void ConvertIndex(_uint AllIndex, _uint* CellIndex, _uint* PointIndex = nullptr);
	void Reset_BoundingSpheres();
	
#endif

private:
	/* 이 네비게이션 컴포넌트를 이용하는 객체가 어떤 쎌안에 있는지?! */
	NAVIGATIONDESC					m_NaviDesc;
	vector<class CCell*>			m_Cells;
	vector<Triangle>				m_TriangleList;

#ifdef _DEBUG
private:
	class CShader*					m_pShader = { nullptr };
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

	vector<BoundingSphere> m_BoundingSpheres;
	_bool			m_bRender = { true };
	
#endif

private:
	HRESULT SetUp_Neighbors();

public:	
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	void Save_Navigation(const wstring& name);
};

END