#include "stdafx.h"
#include "ImGuiManager.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "GameInstance.h"
#include "ImGuiWindow.h"
#include "MainWindow.h"
#include <MapWindow.h>
#include <LogWindow.h>
#include <ObjectWindow.h>
#include <UIWindow.h>
#include "InspectorWindow.h"

IMPLEMENT_SINGLETON(CImGuiManager);

HRESULT CImGuiManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	m_pDevice->AddRef();
	m_pContext->AddRef();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoDefaultParent = true;

	
	
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);

	
	m_WindowsMap[MAIN_WINDOW_NAME] = CMainWindow::Create(WINDOWDESC(MAIN_WINDOW_NAME, false, false, { 6.f,-100.f }, false, {1123.f,134.f}));

	m_WindowsMap[MAP_WINDOW_NAME] = CMapWindow::Create(WINDOWDESC(MAP_WINDOW_NAME, false, false, { -405.f,-100.f }, false, { 412.f,913.f }));

	m_WindowsMap[OBJECT_WINDOW_NAME] = CObjectWindow::Create(WINDOWDESC(OBJECT_WINDOW_NAME, false, false, { -405.f,-100.f }, false, { 412.f,500.f }));

	m_WindowsMap[INSPECTOR_WINDOW_NAME] = CInspectorWindow::Create(WINDOWDESC(INSPECTOR_WINDOW_NAME, false, false, { -405.f,400.f }, false, { 412.f,413.f }));


	m_WindowsMap[UI_WINDOW_NAME] = CUIWindow::Create(WINDOWDESC(UI_WINDOW_NAME, false, false, { -405.f,-100.f }, false, { 412.f,913.f }));

	m_WindowsMap[LOG_WINDOW_NAME] = CLogWindow::Create(WINDOWDESC(LOG_WINDOW_NAME, false, false, { 6.f,+612.f }, false, { 1123.f,200.f }));

	ShowWindow(MAP_WINDOW_NAME);
	ShowWindow(MAIN_WINDOW_NAME);
	ShowWindow(LOG_WINDOW_NAME);
	/*HideAllWindows();
	ShowWindow(MAIN_WINDOW_NAME);*/

	return S_OK;
}

void CImGuiManager::Tick(_double TimeDelta)
{
	for (auto& item : m_WindowsMap)
		item.second->Tick(TimeDelta);
}

void CImGuiManager::Render()
{
	if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
	{
		CGameInstance::GetInstance()->ResizeBuffers(g_ResizeWidth, g_ResizeHeight);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	for (auto& item : m_WindowsMap)
		item.second->Render();
	
	
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImGuiManager::ShowWindow(const char* WindowName)
{
	auto iter = m_WindowsMap.find(WindowName);

	if (iter == m_WindowsMap.end()) return;

	iter->second->Open_Window();
}

void CImGuiManager::HideWindow(const char* WindowName)
{
	auto iter = m_WindowsMap.find(WindowName);

	if (iter == m_WindowsMap.end()) return;

	iter->second->Open_Window();
}

CImGuiWindow* CImGuiManager::GetWindow(const char* WindowName)
{
	return m_WindowsMap[WindowName];
}

void CImGuiManager::ShowAllWindows()
{
	for (auto& iter : m_WindowsMap)
	{
		iter.second->Open_Window();
	}	
}

void CImGuiManager::HideAllWindows()
{
	for (auto& iter : m_WindowsMap)
	{
		if (!iter.second->IsAlwaysShow())
			iter.second->Close_Window();
	}
		
}

_uint CImGuiManager::GetInspectorMode()
{
	CObjectWindow* objectWindow = dynamic_cast<CObjectWindow*>(m_WindowsMap[OBJECT_WINDOW_NAME]);
	if (objectWindow == nullptr) return IM_DEFAULT;

	return objectWindow->GetInspectorMode();
}

wstring CImGuiManager::GetCurSelectName()
{
	CObjectWindow* objectWindow = dynamic_cast<CObjectWindow*>(m_WindowsMap[OBJECT_WINDOW_NAME]);
	if (objectWindow == nullptr) return nullptr;

	return objectWindow->GetCurSelectName();
}

CComponent* CImGuiManager::GetCurSelectComponent()
{
	CObjectWindow* objectWindow = dynamic_cast<CObjectWindow*>(m_WindowsMap[OBJECT_WINDOW_NAME]);
	if (objectWindow == nullptr) return nullptr;

	return objectWindow->GetCurSelectComponent();
}

void CImGuiManager::Free()
{
	for (auto it = m_WindowsMap.begin(); it != m_WindowsMap.end(); it++)
		Safe_Release(it->second);
	
	m_WindowsMap.clear();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}