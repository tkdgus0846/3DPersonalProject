#pragma once

#include "Component.h"

/* �׺���̼��� �����ϱ����� CCell(�ﰢ��)���� ��� �ִ´�.  */

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

	// ���� �̵����� ���� ��ġ�� ������ �־��ָ� �����̵� ���͸� �̾��ش�.
	_int SlidingVector(_uint& index, _fvector pos, _fvector dir, _float3& slidingVector, map<_uint, _uint>& history);
	_vector Get_Normal(_int neighbor);
	

#ifdef _DEBUG
public:
	HRESULT Render_Navigation();
	
	vector<BoundingSphere>* Get_All_Sphere();

	// ������ ���� ��������. � �����ִ� Picking_Point�� �ҷ��ٰǰ�?
	_float3 Picking_Cell(_uint index);

	void Reset_Selected_AllCells();

	void Translate(_uint index, _fvector trans);
	void Set_Render(_bool bRender) { m_bRender = bRender; }
	

private:
	void ConvertIndex(_uint AllIndex, _uint* CellIndex, _uint* PointIndex = nullptr);
	void Reset_BoundingSpheres();
	
#endif

private:
	/* �� �׺���̼� ������Ʈ�� �̿��ϴ� ��ü�� � ��ȿ� �ִ���?! */
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