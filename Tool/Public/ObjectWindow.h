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
	void			ChangeTree() { m_bChangeTree = true; }
	CGameObject*	Find_GameObject(const wstring& name);

	void			Picking_LoadObject();

	virtual _bool	Is_Use_Picking() final { return false; }

private:
	virtual void	Rendering() override;
	void			MakeTree();
	void			ShowTree(const list<CComponent*>& compList, _bool root = true);	
	void			SelectInspectorMode(const wstring& name);

	virtual void	Save_Function() final;
	virtual void	Load_Function() final;

private:
	list<CComponent*>		m_GameObjects;
	_bool					m_bChangeTree = { true };

	_uint					m_eInspectorMode;
	wstring					m_pCurSelectName = { L"" };
	CComponent*				m_pCurSelectComponent = { nullptr };

	
};

