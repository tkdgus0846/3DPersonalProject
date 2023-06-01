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
		string name = CConversion::TcharToString(item.first);
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
	

	const _tchar* name = IMGUI->GetCurSelectName();

	NewLine();

	if (name != nullptr)
	{
		SeparatorText(CConversion::TcharToString(name).c_str());
	}
		
	switch (IMGUI->GetInspectorMode())
	{
	case IM_GAMEOBJECT:
		Mode_GameObject();
		break;
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
	case IM_DEFAULT:
		Mode_Default();
		break;
	default:
		break;
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
	SameLine();
	if (ImGui::Button("Delete Component", ImVec2(180, 50)))
	{
		Delete_Component_Function();
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

void CInspectorWindow::Mode_Default()
{
	if (ImGui::Button("Add GameObject", ImVec2(180, 50)))
	{

	}
	SameLine();
	if (ImGui::Button("Delete GameObject", ImVec2(180, 50)))
	{

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

	for (auto& str : splitVector)
	{
		if (str.compare("Renderer") == 0)
		{
			if (gameObject->m_pRendererCom != nullptr) 
				return;

			FieldComp = (CComponent**)&gameObject->m_pRendererCom;
		}
		if (str.compare("Transform") == 0)
		{
			if (gameObject->m_pTransformCom != nullptr) 
				return;

			FieldComp = (CComponent**)&gameObject->m_pTransformCom;

			CTransform::TRANSFORMDESC desc = CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f));
			pArg = (void*)&desc;
		}
		if (str.compare("VIBuffer") == 0)
		{
			if (gameObject->m_pVIBufferCom != nullptr) 
				return;
			FieldComp = (CComponent**)&gameObject->m_pVIBufferCom;
		}
		if (str.compare("Shader") == 0)
		{
			if (gameObject->m_pShaderCom != nullptr)
				return;
			FieldComp = (CComponent**)&gameObject->m_pShaderCom;
		}
		if (str.compare("Texture") == 0)
		{
			if (gameObject->m_pTextureCom != nullptr)
				return;
			FieldComp = (CComponent**)&gameObject->m_pTextureCom;
		}
		
	}

	// Tchar* 변환 다시 처리하기
	gameObject->Add_Component(LEVEL_TOOL, m_PrototypesVec[selectedComponentIndex].first,
		TEXT("Renderer"), (CComponent**)&FieldComp, gameObject);

	//if (L"Prototype_Component_Renderer" == m_PrototypesVec[selectedComponentIndex].first)
	//{
	//	gameObject->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
	//		TEXT("Renderer"), (CComponent**)&gameObject->m_pRendererCom, gameObject);
	//}
	//if (L"Prototype_Component_Transform" == m_PrototypesVec[selectedComponentIndex].first)
	//{
	//	/* For.Com_Transform */
	//	
	//	gameObject->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Transform"),
	//		TEXT("Transform"), (CComponent**)&gameObject->m_pTransformCom, gameObject, &desc);
	//}
	//if (L"Prototype_Component_VIBuffer_Terrain" == m_PrototypesVec[selectedComponentIndex].first)
	//{
	//	///* For.Com_VIBuffer */
	//	gameObject->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//		TEXT("VIBuffer"), (CComponent**)&gameObject->m_pVIBufferCom, gameObject);
	//}
	//if (L"Prototype_Component_Shader_VtxNorTex" == m_PrototypesVec[selectedComponentIndex].first)
	//{
	//	///* For.Com_Shader */
	//	gameObject->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//		TEXT("Shader"), (CComponent**)&gameObject->m_pShaderCom, gameObject);
	//}
	//if (L"Prototype_Component_Texture_Terrain" == m_PrototypesVec[selectedComponentIndex].first)
	//{
	//	///* For.Com_Texture */
	//	gameObject->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
	//		TEXT("Texture"), (CComponent**)&gameObject->m_pTextureCom, gameObject);
	//}

	

	


	
}

void CInspectorWindow::Delete_Component_Function()
{
	if (selectedComponentIndex < 0 || selectedComponentIndex >= m_PrototypesVec.size()) return;
}


