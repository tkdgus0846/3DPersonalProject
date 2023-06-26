#include "stdafx.h"
#include "..\Public\AnimWindow.h"
#include "GameInstance.h"
#include "DummyObject.h"
#include <Camera_Main.h>
#include "Animation.h"

HRESULT CAnimWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);

	no_titlebar = false;
	m_bAlwaysShow = false;
	save_load_button = false;

	m_ModelPrototypesVec = CGameInstance::GetInstance()->Get_Prototypes_ByVector();

	for (auto& item : m_ModelPrototypesVec)
	{
		string name = CConversion::WstringToString(item.first);
		m_ModelPrototypesStrVec.push_back(name);
	}

	auto iter1 = m_ModelPrototypesVec.begin();
	auto iter2 = m_ModelPrototypesStrVec.begin();
	while (true)
	{
		if ((*iter2).find("_Anim_Model") != std::string::npos)
		{
			iter1++;
			iter2++;
		}
		else
		{
			iter1 = m_ModelPrototypesVec.erase(iter1);
			iter2 = m_ModelPrototypesStrVec.erase(iter2);
		}
		if (iter1 == m_ModelPrototypesVec.end()) break;
	}

	return result;
}

CAnimWindow* CAnimWindow::Create(const WINDOWDESC& windowDesc)
{
	CAnimWindow* newWindow = new CAnimWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("CLogWindow Create Failed!");
		return nullptr;
	}
	return newWindow;
}

void CAnimWindow::Rendering()
{
	Show_ModelPrototypes();
	Show_Animations();
}



void CAnimWindow::Show_ModelPrototypes()
{
	std::vector<int> searchResults;

	ImGui::InputText("SearchModel", m_ModelSearchBuffer, IM_ARRAYSIZE(m_ModelSearchBuffer));
	ImGui::BeginChild("ScrollRegionModel", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	for (int i = 0; i < m_ModelPrototypesStrVec.size(); i++)
	{
		if (strstr(m_ModelPrototypesStrVec[i].c_str(), m_ModelSearchBuffer) != nullptr) // 검색어와 일치하는 경우에만 항목을 표시
		{
			searchResults.push_back(i);
		}
	}

	for (int i = 0; i < searchResults.size(); i++)
	{
		int index = searchResults[i];
		if (ImGui::Selectable(m_ModelPrototypesStrVec[index].c_str()))
		{
			if (selectedModelIndex != index)
			{
				selectedModelIndex = index;

				wstring name = L"AnimDummy";
				CGameInstance* pGameInstance = CGameInstance::GetInstance();

				pGameInstance->Delete_GameObject(L"Layer_Object", name);

				m_DummyObject = (CDummyObject*)pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyObject"), TEXT("Layer_Object"), name);

				m_DummyObject->Add_Component(LEVEL_TOOL, L"Prototype_Component_Shader_VtxAnimMesh", SHADER_W, (CComponent**)&m_DummyObject->m_pShaderCom);

				m_DummyObject->Add_Component(LEVEL_TOOL, CConversion::StringToWstring(m_ModelPrototypesStrVec[index]).c_str(), MODEL_W, (CComponent**)&m_DummyObject->m_pModelCom);

				m_DummyObject->m_pTransformCom->Set_Position({ -100.f, -10099.f, -100.f, 1.f }); // 이것보다 z 10 앞에 있게 만들자.
			}
			
		}
	}

	ImGui::EndChild();

	if (selectedModelIndex >= 0 && selectedModelIndex < m_ModelPrototypesStrVec.size())
	{
		NewLine();
		BulletText(m_ModelPrototypesStrVec[selectedModelIndex].c_str());
		NewLine();
	}
}

void CAnimWindow::Show_Animations()
{
	if (selectedModelIndex < 0) return;
	if (m_DummyObject == nullptr) return;

	vector<class CAnimation*>* animations = m_DummyObject->m_pModelCom->Get_Animations();

	//ImGui::Begin("Table");
	ImGui::Columns(3, "myColumns"); // 3개의 열 생성

	// 열 헤더
	ImGui::Text("AnimIndex");
	ImGui::NextColumn();
	ImGui::Text("AnimationName");
	ImGui::NextColumn();
	ImGui::Text("isLoop");
	ImGui::NextColumn();

	
	if (CGameInstance::GetInstance()->Key_Down(DIK_DOWNARROW))
	{
		m_CurAnimationIndex++;
		if (m_CurAnimationIndex > (*animations).size())
			m_CurAnimationIndex = 0;
		m_DummyObject->m_pModelCom->Set_AnimIndex(m_CurAnimationIndex);
	}
	if (CGameInstance::GetInstance()->Key_Down(DIK_UPARROW))
	{
		m_CurAnimationIndex--;
		if (m_CurAnimationIndex < 0)
			m_CurAnimationIndex = animations->size() - 1;
		m_DummyObject->m_pModelCom->Set_AnimIndex(m_CurAnimationIndex);
	}
		

	_int index = 0;
	for (CAnimation* anim : (*animations))
	{
		if (ImGui::Selectable(to_string(index).c_str(), index == m_CurAnimationIndex))
		{
			m_CurAnimationIndex = index;
			m_DummyObject->m_pModelCom->Set_AnimIndex(index);
		}
		//ImGui::Separator();

		// 행 데이터
		//ImGui::Text(to_string(index).c_str());
		SameLine();
		ImGui::NextColumn();
		ImGui::Text(anim->Get_Name());
		ImGui::NextColumn();
		if (anim->Get_Loop())
			ImGui::Text("true");
		else
			ImGui::Text("false");
		ImGui::NextColumn();
		index++;
	}
	//ImGui::End();
	
	//m_ModelPrototypesVec[selectedModelIndex]
}

void CAnimWindow::Open_Function()
{
	CGameObject* object = CGameInstance::GetInstance()->Find_GameObject(TEXT("Layer_Camera"), L"MainCamera");
	CCamera_Main* mainCamera = dynamic_cast<CCamera_Main*>(object);

	mainCamera->ChooseType(CCamera_Main::TYPE_ANIM);
}

void CAnimWindow::Close_Function()
{
	CGameObject* object = CGameInstance::GetInstance()->Find_GameObject(TEXT("Layer_Camera"), L"MainCamera");
	CCamera_Main* mainCamera = dynamic_cast<CCamera_Main*>(object);

	mainCamera->ChooseType(CCamera_Main::TYPE_MAIN);
}
