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
	_bool is_Move(_fvector vPosition);

#ifdef _DEBUG
public:
	HRESULT Render_Navigation();
#endif

private:
	/* 이 네비게이션 컴포넌트를 이용하는 객체가 어떤 쎌안에 있는지?! */
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