#include "stdafx.h"
#include "ImGuiWindow.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

CImGuiWindow::CImGuiWindow()
{
}

HRESULT CImGuiWindow::Initialize(const WINDOWDESC& desc)
{
	m_Desc = desc;
	no_collapse = true;
	return S_OK;
}

void CImGuiWindow::Render()
{
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;

	//if (cond_always)			window_flags |= ImGuiCond_Appearing;
	//if (no_close)           p_open = NULL; // Don't pass our bool* to Begin
	
	if (m_Desc.bOpen == false) return;

	RECT rct;
	GetWindowRect(g_hWnd, &rct);

	ImVec2 windowPos = { (float)rct.left, (float)rct.top };

	if (m_Desc.bCanMove == false)
		ImGui::SetNextWindowPos(windowPos + m_Desc.Pos); // pivot 의 디폴트는 0,0 좌측상단.

	if (m_Desc.bCanResizable == false)
	{
		SetNextWindowSize(m_Desc.Size);
	}
	else
	{
		if (m_Desc.bSizeConstraints == true)
			SetNextWindowSizeConstraints(m_Desc.MinSize, m_Desc.MaxSize);
	}
		
	ImGui::Begin(m_Desc.pWindowName, nullptr, window_flags);

	Rendering();
	if (save_load_button)
		SaveLoadButton();
	ExitButton();


	ImGui::End();
}

void CImGuiWindow::Open_Window()
{
	Open_Function();
	m_Desc.bOpen = true;
}

void CImGuiWindow::Close_Window()
{
	Close_Function();
	m_Desc.bOpen = false;
}

void CImGuiWindow::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void CImGuiWindow::Save_Function()
{
}

void CImGuiWindow::Load_Function()
{
}

void CImGuiWindow::SaveLoadButton()
{
	NewLine();
	NewLine();
	
	string str = m_Desc.pWindowName;
	str = str + "  Save & Load";
	SeparatorText(str.c_str());


	PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 버튼 색상 변경	
	PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	
	if (only_load_button == false)
	{
		if (Button("Save", { 50,50 }))
		{
			Save_Function();
		}
		SameLine();
	}
	if (Button("Load", { 50,50 }))
	{
		Load_Function();
	}
	ImGui::PopStyleColor(); // 이전의 스타일로 복원
	ImGui::PopStyleColor(); // 이전의 스타일로 복원

}


void CImGuiWindow::Free()
{
}


