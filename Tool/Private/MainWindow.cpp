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
	Render_Mode();

	SetCursorPosX(10.f);
	SetCursorPosY(10.f);

	PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 1.0f)); // ��ư ���� ����	
	PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

	if (Button("MapTool", BUTTON_SIZE))
	{
		IMGUI->HideAllWindows(MAP_WINDOW_NAME);
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

	if (Button("AnimTool", BUTTON_SIZE))
	{
		IMGUI->HideAllWindows();
		IMGUI->ShowWindow(ANIM_WINDOW_NAME);
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
	PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // ��ư ���� ����	
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

	ImGui::PopStyleColor(); // ������ ��Ÿ�Ϸ� ����
	ImGui::PopStyleColor(); // ������ ��Ÿ�Ϸ� ����
}

void CMainWindow::ExitButton()
{
	SameLine();
	PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // ��ư ���� ����	
	PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

	//SetNextWindowPos(ImVec2(0, 0));

	if (Button("Exit", BUTTON_SIZE))
	{
		PostQuitMessage(0);
	}

	ImGui::PopStyleColor(); // ������ ��Ÿ�Ϸ� ����
	ImGui::PopStyleColor(); // ������ ��Ÿ�Ϸ� ����
}

void CMainWindow::Render_Mode()
{
	SetCursorPosX(900.f);
	if (RadioButton("NavMesh	  ON/OFF", m_bNavMeshRender))
	{
		m_bNavMeshRender = (m_bNavMeshRender == false) ? true : false;
		IMGUI->Set_Render_NavMesh(m_bNavMeshRender);
	}
	SetCursorPosX(900.f);
	if (RadioButton("WireFrame	ON/OFF", m_bWireFrameRender))
	{
		m_bWireFrameRender = (m_bWireFrameRender == false) ? true : false;
	}

}

void CMainWindow::Save_Function()
{
	//ImGui::OpenPopup("Save As");
}

void CMainWindow::Load_Function()
{
}

