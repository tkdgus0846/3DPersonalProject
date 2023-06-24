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
	void			MakeTree();

	class CDummyObject* Find_Terrain();

	void			Picking_LoadObject();

	virtual _bool	Is_Use_Picking() final { return false; }
	void			PickingReset();

private:
	virtual void	Rendering() override;
	
	void			ShowTree(const list<CComponent*>& compList, _bool root = true);
	//void			Close_All_TreeNode();

	// 피킹처리 관련 함수들
	void			PickingIndex(_int iIndex);
	
	
	_bool			Is_Picking();
	_bool			Is_Close_Picking();
	_bool			Is_Open_Picking();
	void			Watch_Object();
	void			Watch_Main();



	void			SelectInspectorMode(const wstring& name);

	virtual void	Save_Function() final;
	virtual void	Load_Function() final;

private:
	list<CComponent*>		m_GameObjects;
	_bool					m_bChangeTree = { true };

	_uint					m_eInspectorMode;
	wstring					m_pCurSelectName = { L"" };
	CComponent*				m_pCurSelectComponent = { nullptr };

	_int					m_iPickingIndex = { -1 };
	_int					m_iPrePickingIndex = { -1 };

	
};

