#pragma once
#include "ImGuiWindow.h"

BEGIN(Engine)
class CGameObject;
class CComponent;
END

class CObjectWindow : public CImGuiWindow
{

public:
	virtual HRESULT Initialize(const WINDOWDESC& desc) override;
	static CObjectWindow* Create(const WINDOWDESC& windowDesc);
	_uint			GetInspectorMode();
	wstring			GetCurSelectName() { return m_pCurSelectName; }
	CComponent*		GetCurSelectComponent() { return m_pCurSelectComponent; }

private:
	virtual void	Rendering() override;

	void			MakeTree();
	void			ShowTree(const list<CComponent*>& compList, _bool root = true);
	void			ChangeTree() { m_bChangeTree = true; }
	void			SelectInspectorMode(const wstring& name);
	
	

private:
	list<CComponent*>		m_GameObjects;
	_bool					m_bChangeTree = { true };

	_uint					m_eInspectorMode;
	wstring					m_pCurSelectName = { L"" };
	CComponent*				m_pCurSelectComponent = { nullptr };
};

