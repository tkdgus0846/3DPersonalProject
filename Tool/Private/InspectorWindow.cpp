#include "stdafx.h"
#include "InspectorWindow.h"
#include "DummyObject.h"
#include "Transform.h"
#include "GameInstance.h"

HRESULT CInspectorWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);

	no_titlebar = false;
	save_load_button = false;

	m_PrototypesVec = CGameInstance::GetInstance()->Get_Prototypes_ByVector();

	for (auto& item : m_PrototypesVec)
	{
		string name = CConversion::WstringToString(item.first);
		m_PrototypesStrVec.push_back(name);
	}
	

	return result;
}

CInspectorWindow* CInspectorWindow::Create(const WINDOWDESC& windowDesc)
{
	CInspectorWindow* newWindow = new CInspectorWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("CInspectorWindow Create Failed!");
		return nullptr;
	}
	return newWindow;
}

void CInspectorWindow::Rendering()
{
	/*CComponent* comp;
	comp = IMGUI->GetCurSelectComponent();
	if (comp != nullptr)
	{
		if (m_pCurComponent != IMGUI->GetCurSelectComponent())
		{

		}
	}*/
	

	wstring name = IMGUI->GetCurSelectName();

	NewLine();

	if (name.data() != nullptr)
	{
		SeparatorText(CConversion::WstringToString(name).c_str());
	}

	_uint SelectMode = IMGUI->GetInspectorMode();
	if (SelectMode == IM_GAMEOBJECT)
	{
		Mode_GameObject();
	}
	else if (SelectMode == IM_DEFAULT)
	{
		Mode_Default();
	}
	else
	{
		
		switch (SelectMode)
		{
		case IM_TRANSFORM:
			Mode_Transform();
			break;
		case IM_COLLIDER:
			Mode_Collider();
			break;
		case IM_TEXTURE:
			Mode_Texture();
			break;
		case IM_SHADER:
			Mode_Shader();
			break;
		case IM_ANIMATION:
			Mode_Animation();
			break;
		case IM_RENDERER:
			Mode_Renderer();
			break;
		default:
			break;
		}
		Delete_Button();
	}
	
}

void CInspectorWindow::Mode_GameObject()
{
	Spacing();
	Show_ComponentList();
	NewLine();
	
	if (ImGui::Button("Add Component", ImVec2(180,50)))
	{
		Add_Component_Function();
	}
}

void CInspectorWindow::Mode_Transform()
{
	static _float translation[3];
	static _float rotation[3];
	static _float scale[3];
	ImGui::Spacing();
	ImGui::DragFloat3("Translation", translation, 1.f, -99999.f, 99999.f);
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::DragFloat3("Rotation", rotation, 1.f, -99999.f, 99999.f);
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::DragFloat3("Scale", scale, 1.f, -99999.f, 99999.f);
	ImGui::Spacing();
}

void CInspectorWindow::Mode_Collider()
{
	
}

void CInspectorWindow::Mode_Texture()
{
	
}

void CInspectorWindow::Mode_Shader()
{
	
}

void CInspectorWindow::Mode_Animation()
{
	
}

void CInspectorWindow::Mode_Renderer()
{
}

void CInspectorWindow::Mode_Default()
{
	ImGui::SetNextItemWidth(120);
	ImGui::InputText("Object Name", searchBuffer, IM_ARRAYSIZE(searchBuffer));
	if (ImGui::Button("Add GameObject", ImVec2(180, 50)))
	{
		Add_GameObject_Function();
	}
	SameLine();
	if (ImGui::Button("Delete GameObject", ImVec2(180, 50)))
	{
		Delete_GameObject_Function();
	}
}

void CInspectorWindow::Show_ComponentList()
{
	std::vector<int> searchResults;

	ImGui::InputText("Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));
	ImGui::BeginChild("ScrollRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	for (int i = 0; i < m_PrototypesStrVec.size(); i++)
	{
		if (strstr(m_PrototypesStrVec[i].c_str(), searchBuffer) != nullptr) // 검색어와 일치하는 경우에만 항목을 표시
		{
			searchResults.push_back(i);
		}
	}

	for (int i = 0; i < searchResults.size(); i++)
	{	
		int index = searchResults[i];
		if (ImGui::Selectable(m_PrototypesStrVec[index].c_str()))
		{
			selectedComponentIndex = index;
		}
	}

	ImGui::EndChild();

	if (selectedComponentIndex >= 0 && selectedComponentIndex < m_PrototypesStrVec.size())
	{
		NewLine();
		BulletText(m_PrototypesStrVec[selectedComponentIndex].c_str());
	}
		

	//ImGui::ListBox("Prototypes", &selectedItemIndex, items, m_PrototypesStrVec.size());
}

void CInspectorWindow::Add_Component_Function()
{
	if (selectedComponentIndex < 0 || selectedComponentIndex >= m_PrototypesVec.size()) return;

	string str;

	vector<string> splitVector = CConversion::SplitStringA(m_PrototypesStrVec[selectedComponentIndex],"_");
	
	CDummyObject* gameObject = dynamic_cast<CDummyObject*>(IMGUI->GetCurSelectComponent());
	CComponent** FieldComp = nullptr;
	void* pArg = nullptr;
	string name;

	for (auto& str : splitVector)
	{
		if (str.compare("Renderer") == 0)
		{
			if (gameObject->m_pRendererCom != nullptr) 
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pRendererCom;
		}
		if (str.compare("Transform") == 0)
		{
			if (gameObject->m_pTransformCom != nullptr) 
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pTransformCom;

			CTransform::TRANSFORMDESC desc = CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f));
			pArg = (void*)&desc;
		}
		if (str.compare("VIBuffer") == 0)
		{
			if (gameObject->m_pVIBufferCom != nullptr) 
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pVIBufferCom;
		}
		if (str.compare("Shader") == 0)
		{
			if (gameObject->m_pShaderCom != nullptr)
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pShaderCom;
		}
		if (str.compare("Texture") == 0)
		{
			if (gameObject->m_pTextureCom != nullptr)
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pTextureCom;
		}
		
	}

	// Tchar* 변환 다시 처리하기

	wstring componentName = CConversion::StringToWstring(name);
	gameObject->Add_Component(LEVEL_TOOL, m_PrototypesVec[selectedComponentIndex].first,
		componentName.c_str(), (CComponent**)FieldComp, gameObject, pArg);	
}

void CInspectorWindow::Delete_Component_Function()
{
	if (selectedComponentIndex < 0 || selectedComponentIndex >= m_PrototypesVec.size()) return;

	CComponent* component = IMGUI->GetCurSelectComponent();
	if (component == nullptr) return;
	CDummyObject* gameObject = dynamic_cast<CDummyObject*>(component->GetOwner());
	if (gameObject == nullptr) return;
	
	gameObject->Delete_Component(component->GetName().c_str());

	string str = CConversion::WstringToString(component->GetName());
	CComponent** resultCom = nullptr;
	if (str.compare("Renderer") == 0)
	{
		resultCom = (CComponent**)&gameObject->m_pRendererCom;
	}
	if (str.compare("Transform") == 0)
	{
		resultCom = (CComponent**)&gameObject->m_pTransformCom;
	}
	if (str.compare("VIBuffer") == 0)
	{
		resultCom = (CComponent**)&gameObject->m_pVIBufferCom;
	}
	if (str.compare("Shader") == 0)
	{
		resultCom = (CComponent**)&gameObject->m_pShaderCom;
	}
	if (str.compare("Texture") == 0)
	{
		resultCom = (CComponent**)&gameObject->m_pTextureCom;
	}
	
	if (resultCom != nullptr && *resultCom != nullptr)
	{
		Safe_Release(*resultCom);
		*resultCom = nullptr;
	}
		
}

void CInspectorWindow::Delete_Button()
{
	NewLine();
	if (ImGui::Button("Delete Component", ImVec2(180, 50)))
	{
		Delete_Component_Function();
	}
}

void CInspectorWindow::Add_GameObject_Function()
{
}

void CInspectorWindow::Delete_GameObject_Function()
{
}


