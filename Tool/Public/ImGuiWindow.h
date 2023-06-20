#pragma once
#include "Base.h"
#include "ImGuiManager.h"

using namespace ImGui;

typedef struct ImGuiWindowDesc
{
	ImGuiWindowDesc() : 
		pWindowName(nullptr), 
		bOpen(true), 
		bCanMove(true), 
		Pos(ImVec2(0, 0)),
		bCanResizable(true),
		Size(ImVec2(0, 0)){}

	ImGuiWindowDesc
	(
		const _char*	_pWindowName, 
		const _bool&	_bOpen = true, 
		const _bool&	_bCanMove = false, 
		const ImVec2&	_Pos = WINDOW_DEFAULT_POS,
		const _bool&	_bCanResizable = true, 
		const ImVec2&	_Size = WINDOW_DEFAULT_SIZE,
		const _bool&	_bSizeConstraints = true,
		const ImVec2&	_MinSize = WINDOW_DEFAULT_MINSIZE,
		const ImVec2&	_MaxSize = WINDOW_DEFAULT_MAXSIZE
	) :
		pWindowName(_pWindowName), 
		bOpen(_bOpen), 
		bCanMove(_bCanMove), 		
		Pos(_Pos),
		bCanResizable(_bCanResizable),
		Size(_Size),
		bSizeConstraints(_bSizeConstraints),
		MinSize(_MinSize),
		MaxSize(_MaxSize) {}

	const _char*	pWindowName;
	_bool			bOpen;
	_bool			bCanMove;
	
	ImVec2			Pos;
	_bool			bCanResizable;
	ImVec2			Size;

	_bool			bSizeConstraints;
	ImVec2			MinSize;
	ImVec2			MaxSize;

}WINDOWDESC;

class CImGuiWindow : public CBase
{
public:
	CImGuiWindow();
	virtual ~CImGuiWindow() = default;

	virtual HRESULT Initialize(const WINDOWDESC& desc);
	virtual void Tick(_double timeDelta) {}
	void Render();

	void Open_Window();
	void Close_Window();

	_bool IsAlwaysShow() const { return m_bAlwaysShow; }

	virtual _bool Is_Use_Picking() { return false; }
	
protected:
	//virtual void PreRender() PURE;
	virtual void Rendering() PURE;

	void HelpMarker(const char* desc);

	virtual void Open_Function() {}
	virtual void Close_Function() {}
	
	virtual void Save_Function();
	virtual void Load_Function();

private:
	virtual void SaveLoadButton();
	virtual void Free() override;

	virtual void ExitButton() {}

protected:
	WINDOWDESC			m_Desc;
	_bool				m_bAlwaysShow = false;

protected:
	ImGuiWindowFlags	window_flags = 0;
	_bool				no_titlebar = true;
	_bool				no_scrollbar = false;
	_bool				no_menu = true;
	_bool				no_move = false;
	_bool				no_resize = true;
	_bool				no_collapse = false;
	_bool				no_close = false;
	_bool				no_nav = false;
	_bool				no_background = false;
	_bool				no_bring_to_front = false;
	_bool				unsaved_document = false;
	_bool				cond_always = true;
	_bool				save_load_button = true;

	
};

