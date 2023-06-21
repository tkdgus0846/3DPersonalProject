#include "stdafx.h"
#include "..\Public\ObjectWindow.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "InspectorWindow.h"
#include "Calculator.h"
#include <DummyObject.h>
#include <Camera_Main.h>

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

void CObjectWindow::Picking_LoadObject()
{
	if (IMGUI->Window_Use_Picking(INSPECTOR_WINDOW_NAME))
		return;

	vector<BoundingSphere> BoundingSpheres;
	BoundingSpheres.resize(m_GameObjects.size());

	_int loopIndex = -1;
	for (auto& comp : m_GameObjects)
	{
		loopIndex++;
		CDummyObject* object = dynamic_cast<CDummyObject*>(comp);
		if (object == nullptr) continue;
		
		BoundingSpheres[loopIndex] = *object->Get_PickingSphere();
	}
	_float4 resultPos;
	_int iPickingIndex;

	iPickingIndex = CCalculator::Picking_Sphere(g_hWnd, g_iWinSizeX, g_iWinSizeY, &BoundingSpheres, &resultPos);

	if (iPickingIndex == -1)
	{
		return;
	}

	PickingIndex(iPickingIndex);

	// 여기다가 picking 됐을때 트리를 처리해주는 코드를 넣어야됨

}

void CObjectWindow::Rendering()
{
	m_eInspectorMode = IM_DEFAULT;
	m_pCurSelectName = L"";
	m_pCurSelectComponent = nullptr;

	if (CGameInstance::GetInstance()->Mouse_Down(CInput_Device::DIMK_LBUTTON))
		Picking_LoadObject();

	
	

	if (ImGui::CollapsingHeader("Hierarchy"))
	{
		MakeTree();
		
		ShowTree(m_GameObjects);
	}

	if (Is_Picking())
	{
		NewLine();
		if (Button("Watch"))
		{
			Watch_Object();
		}
		SameLine();
		if (Button("Cancel"))
		{
			PickingReset();
			Watch_Main();
		}
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
	_int index = -1;
	for (CComponent* obj : compList)
	{
		if (root == true) 
			index++;

		if (index != -1 && m_iPickingIndex != -1 && index != m_iPickingIndex)
			continue;

		wstring name = obj->GetName();

		/*std::wstring_convert<std::codecvt_utf8_utf16<_tchar>, _tchar> converter;
		std::string strString = converter.to_bytes(name);*/
		string strString = CConversion::WstringToString(name);
		//TreeNode("");

		if (Is_Close_Picking() && root == true)
			ImGui::SetNextItemOpen(false);
		if (Is_Open_Picking() && root == true)
			ImGui::SetNextItemOpen(true);

		bool treeRet = TreeNode(strString.c_str());
		// TRUE 일경우에 TreePop 을 해야한다. FALSE일 경우엔 하지 않음.
		// TRUE로 만들고 Pop 을 한다는건 여는것
		
		if (treeRet == false) continue;
		if (name.data() == nullptr) MSG_BOX("이름 null");

		if (root == true)
			PickingIndex(index);


		m_pCurSelectComponent = obj;
		CComposite* comp = dynamic_cast<CComposite*>(obj);
		
		// 여기 다시 작성해야됨 이름을 게임오브젝트로 넘겨줄 방법을 강구해야되는데 _bool root 까지넘겨준상태임
		SelectInspectorMode(name);

		if (comp != nullptr)
		{
			ShowTree(comp->Get_ComponentsByList(), false);
		}	
		
		if (treeRet)
			TreePop();
	}
}

void CObjectWindow::PickingIndex(_int iIndex)
{
	m_iPrePickingIndex = m_iPickingIndex;
	m_iPickingIndex = iIndex;
}

void CObjectWindow::PickingReset()
{
	PickingIndex(-1);
}

_bool  CObjectWindow::Is_Picking()
{
	return (m_iPickingIndex == -1) ? false : true;
}

_bool  CObjectWindow::Is_Close_Picking()
{
	return (m_iPrePickingIndex != -1) && (m_iPickingIndex == -1);
}

_bool CObjectWindow::Is_Open_Picking()
{
	return (m_iPickingIndex != -1);
}

void  CObjectWindow::Watch_Object()
{
	CGameObject* object = CGameInstance::GetInstance()->Find_GameObject(TEXT("Layer_Camera"), L"MainCamera");
	CCamera_Main* mainCamera = dynamic_cast<CCamera_Main*>(object);
	CDummyObject* dummyObject; 
	CComponent* comp = m_pCurSelectComponent;

	while (true)
	{
		dummyObject = dynamic_cast<CDummyObject*>(comp);
		if (dummyObject == nullptr)
			comp = comp->GetOwner();
		else
			break;
	}
	


	_float4x4 worldMatrix = dummyObject->m_pTransformCom->Get_WorldFloat4x4();
	mainCamera->ChooseType(CCamera_Main::TYPE_OBJECT, XMLoadFloat4x4(&worldMatrix));
	
}

void CObjectWindow::Watch_Main()
{
	CGameObject* object = CGameInstance::GetInstance()->Find_GameObject(TEXT("Layer_Camera"), L"MainCamera");
	CCamera_Main* mainCamera = dynamic_cast<CCamera_Main*>(object);

	mainCamera->ChooseType(CCamera_Main::TYPE_MAIN);
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
	else if (name == L"Model")
	{
		m_eInspectorMode = IM_MODEL;
	}
	else
	{
		m_eInspectorMode = IM_GAMEOBJECT;
	}
}

void CObjectWindow::Save_Function()
{
	IMGUI->SaveMode();
	IMGUI->SaveLoad_By(OBJECT_WINDOW_NAME);
	
	ImGuiFileDialog::Instance()->OpenDialog(IMGUI->SaveLoad_Dialog_Key().c_str(), "Choose File", nullptr, "../../Levels/");
		
}

void CObjectWindow::Load_Function()
{
	IMGUI->LoadMode();
	IMGUI->SaveLoad_By(OBJECT_WINDOW_NAME);
	
	ImGuiFileDialog::Instance()->OpenDialog(IMGUI->SaveLoad_Dialog_Key().c_str(), "Choose File", nullptr, "../../Levels/");
		
}

_uint CObjectWindow::GetInspectorMode()
{
	return m_eInspectorMode;
}


