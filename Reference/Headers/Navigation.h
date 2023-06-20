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
	_bool is_Move(_fvector vPosition);

#ifdef _DEBUG
public:
	HRESULT Render_Navigation();
#endif

private:
	/* �� �׺���̼� ������Ʈ�� �̿��ϴ� ��ü�� � ��ȿ� �ִ���?! */
	NAVIGATIONDESC					m_NaviDesc;
	vector<class CCell*>			m_Cells;

#ifdef _DEBUG
private:
	class CShader*					m_pShader = { nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();

public:	
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END