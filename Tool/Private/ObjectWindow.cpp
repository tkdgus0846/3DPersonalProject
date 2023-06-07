#include "stdafx.h"
#include "..\Public\ObjectWindow.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "InspectorWindow.h"

HRESULT CObjectWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);

	m_eInspectorMode = IM_DEFAULT;

	return result;
}

CObjectWindow* CObjectWindow::Create(const WINDOWDESC& windowDesc)
{
	CObjectWindow* newWindow = new CObjectWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("CObjectWindow Create Failed!");
		return nullptr;
	}
	return newWindow;
}

CGameObject* CObjectWindow::Find_GameObject(const wstring& name)
{
	for (CComponent* component : m_GameObjects)
	{
		if (lstrcmp(name.c_str(), component->GetName().c_str()) == 0)
			return dynamic_cast<CGameObject*>(component);
	}
	return nullptr;
}

void CObjectWindow::Rendering()
{
	m_eInspectorMode = IM_DEFAULT;
	m_pCurSelectName = L"";
	m_pCurSelectComponent = nullptr;

	if (ImGui::CollapsingHeader("Hierarchy"))
	{
		MakeTree();
		
		ShowTree(m_GameObjects);
	}
}

void CObjectWindow::MakeTree()
{
	if (m_bChangeTree == false) return;

	CGameInstance* gameInstance = CGameInstance::GetInstance();
	Safe_AddRef(gameInstance);

	m_GameObjects.clear();

	list<CGameObject*> GameObjectsList = gameInstance->Get_All_Objects();

	for (auto object : GameObjectsList)
	{
		m_GameObjects.push_back(object);
	}

	Safe_Release(gameInstance);

	m_bChangeTree = false;
}

void CObjectWindow::ShowTree(const list<CComponent*>& compList, _bool root)
{
	for (CComponent* obj : compList)
	{
		wstring name = obj->GetName();

		/*std::wstring_convert<std::codecvt_utf8_utf16<_tchar>, _tchar> converter;
		std::string strString = converter.to_bytes(name);*/
		string strString = CConversion::WstringToString(name);
		//TreeNode("");

		bool treeRet = TreeNode(strString.c_str());
		
		if (treeRet == false) continue;
		if (name.data() == nullptr) MSG_BOX("이름 null");


		m_pCurSelectComponent = obj;
		CComposite* comp = dynamic_cast<CComposite*>(obj);
		
		// 여기 다시 작성해야됨 이름을 게임오브젝트로 넘겨줄 방법을 강구해야되는데 _bool root 까지넘겨준상태임
		SelectInspectorMode(name);

		if (comp != nullptr)
		{
			ShowTree(comp->Get_ComponentsByList());
		}	
		
		if (treeRet)
			TreePop();
	}
}

void CObjectWindow::SelectInspectorMode(const wstring& name)
{

	if (name == L"Transform")
	{
		m_eInspectorMode = IM_TRANSFORM;
	}
	else if (name == L"Shader")
	{
		m_eInspectorMode = IM_SHADER;
	}
	else if (name == L"Texture")
	{
		m_eInspectorMode = IM_TEXTURE;
	}
	else if (name == L"VIBuffer")
	{
		m_eInspectorMode = IM_VIBUFFER;
	}
	else if (name == L"Renderer")
	{
		m_eInspectorMode = IM_RENDERER;
	}
	else
	{
		m_eInspectorMode = IM_GAMEOBJECT;
	}
}

_uint CObjectWindow::GetInspectorMode()
{
	return m_eInspectorMode;
}


