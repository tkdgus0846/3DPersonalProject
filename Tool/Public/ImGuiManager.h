#pragma once
#include "Base.h"
#include "Tool_Defines.h"

#define WINDOW_DEFAULT_SIZE		{300.f, g_iWinSizeY-100.f}
#define WINDOW_DEFAULT_POS		{0.f, 0.f}
#define WINDOW_DEFAULT_MINSIZE	{ 0.f,600.f }
#define WINDOW_DEFAULT_MAXSIZE	{300.f, 600.f}

#define MAIN_WINDOW_NAME			"MainWindow"
#define MAP_WINDOW_NAME				"MapTool"
#define OBJECT_WINDOW_NAME			"ObjectTool"
#define UI_WINDOW_NAME				"UI Tool"
#define LOG_WINDOW_NAME				"Log"
#define INSPECTOR_WINDOW_NAME		"Inspector"
#define ANIM_WINDOW_NAME			"AnimWindow"

BEGIN(Engine)
class CComponent;
class CGameObject;
END

class CImGuiManager : public CBase 
{
	DECLARE_SINGLETON(CImGuiManager)

public:
	CImGuiManager() = default;
	~CImGuiManager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	void	Tick(_double TimeDelta);
	void	Render();

	void				ShowWindow(const char* WindowName);
	void				HideWindow(const char* WindowName);
	class CImGuiWindow* GetWindow(const char* WindowName);
	void				ShowAllWindows();
	void				HideAllWindows(const char* WindowName = nullptr);

	/// 오브젝트 윈도우 hierarchy 새로고침
	void				ChangeTree();
	void				PickingReset();

	/// 인스펙터로 값을 넘기기위한 정보
	_uint				GetInspectorMode();
	wstring				GetCurSelectName();
	CComponent*			GetCurSelectComponent();
	CGameObject*		Find_GameObject(const wstring& name);

	// 렌더모드 설정하기 위한 함수
	void				Set_Render_NavMesh(_bool bRender);

	// 맵 윈도우 함수들
	void				Set_CurTerrain(class CDummyObject* pTerrain);

	vector<CGameObject*> Find_Terrains();

	string				SaveLoad_Dialog_Key() const { return m_pSaveLoadDlgKey; }
	void				SaveMode() { m_bSaveMode = true; }
	void				LoadMode() { m_bSaveMode = false; }
	void				SaveLoad_By(const _char* pWindowName)
	{
		m_pCurSaveLoadWindowName = pWindowName;
	}
	const _char* Get_CurSaveLoadWindowName() const
	{
		return m_pCurSaveLoadWindowName;
	}
	_bool Window_Use_Picking(const char* windowName);

	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pContext; }

private:
	virtual void		Free() override;

	void				SaveLoad_Dialog_Function();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	map<const char*, class CImGuiWindow*> m_WindowsMap;

	string					m_pSaveLoadDlgKey = "Save&Load";
	_bool					m_bSaveMode = false;
	const _char*			m_pCurSaveLoadWindowName = "";
};

#define IMGUI CImGuiManager::GetInstance()
#define OPEN_MAIN_WINDOW									   \
	CImGuiManager::GetInstance()->HideAllWindows();			   \
	CImGuiManager::GetInstance()->ShowWindow(MAIN_WINDOW_NAME) \

