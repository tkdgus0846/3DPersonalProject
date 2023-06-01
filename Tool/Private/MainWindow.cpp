#include "stdafx.h"
#include "..\Public\MainWindow.h"

#define	BUTTON_SIZE	{100,50}

CMainWindow* CMainWindow::Create(const WINDOWDESC& windowDesc)
{
	CMainWindow* newWindow = new CMainWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("MainWindow Create Failed!");
		return nullptr;
	}
	return newWindow;



	
}

HRESULT CMainWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);

	m_bAlwaysShow = true;

	return result;
}

void CMainWindow::Rendering()
{
	PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)); // 버튼 색상 변경	
	PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

	if (Button("MapTool", BUTTON_SIZE))
	{
		IMGUI->HideAllWindows();
		IMGUI->ShowWindow(MAP_WINDOW_NAME);
	}
	SameLine();

	if (Button("ObjectTool", BUTTON_SIZE))
	{
		IMGUI->HideAllWindows();
		IMGUI->ShowWindow(OBJECT_WINDOW_NAME);
		IMGUI->ShowWindow(INSPECTOR_WINDOW_NAME);
	}

	SameLine();

	if (Button("UI_Tool", BUTTON_SIZE))
	{
		IMGUI->HideAllWindows();
		IMGUI->ShowWindow(UI_WINDOW_NAME);
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor(); 
	
}

void CMainWindow::SaveLoadButton()
{
	SameLine();
	PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 버튼 색상 변경	
	PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

	if (Button("Save All", BUTTON_SIZE))
	{
		Save_Function();
	}
	SameLine();
	if (Button("Load All", BUTTON_SIZE))
	{
		Load_Function();
	}
	SameLine();

	ImGui::PopStyleColor(); // 이전의 스타일로 복원
	ImGui::PopStyleColor(); // 이전의 스타일로 복원
}

void CMainWindow::ExitButton()
{
	SameLine();
	PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 버튼 색상 변경	
	PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

	//SetNextWindowPos(ImVec2(0, 0));

	if (Button("Exit", BUTTON_SIZE))
	{
		PostQuitMessage(0);
	}

	ImGui::PopStyleColor(); // 이전의 스타일로 복원
	ImGui::PopStyleColor(); // 이전의 스타일로 복원
}

void CMainWindow::Save_Function()
{
	//ImGui::OpenPopup("Save As");
}

void CMainWindow::Load_Function()
{
}

