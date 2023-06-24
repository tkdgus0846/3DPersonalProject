#include "stdafx.h"
#include "InspectorWindow.h"
#include "DummyObject.h"
#include "Transform.h"
#include "Calculator.h"
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

	/*auto iter1 = m_PrototypesVec.begin();
	auto iter2 = m_PrototypesStrVec.begin();
	while (true)
	{
		if ((*iter2).find("Terrain") != std::string::npos)
		{
			iter1 = m_PrototypesVec.erase(iter1);
			iter2 = m_PrototypesStrVec.erase(iter2);
		}
		else
		{
			iter1++;
			iter2++;
		}
		if (iter1 == m_PrototypesVec.end()) break;
	}*/

	m_RenderGroupStrVec.push_back("RENDER_PRIORITY");
	m_RenderGroupStrVec.push_back("RENDER_NONBLEND");
	m_RenderGroupStrVec.push_back("RENDER_NONLIGHT");
	m_RenderGroupStrVec.push_back("RENDER_BLEND");
	m_RenderGroupStrVec.push_back("RENDER_UI");


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
	

	if (IMGUI->GetCurSelectComponent() != nullptr)
	{
		SeparatorText(CConversion::WstringToString(IMGUI->GetCurSelectComponent()->GetName()).c_str());
		NewLine();
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
		case IM_MODEL:
			Mode_Model();
			break;
		case IM_RENDERER:
			Mode_Renderer();
			break;
		case IM_NAVIGATION:
			Mode_Navigation();
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
	PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 버튼 색상 변경		
	if (ImGui::Button("Delete GameObject", ImVec2(200, 30)))
	{
		Delete_GameObject_Function();
	}
	ImGui::PopStyleColor(); // 이전의 스타일로 복원

	NewLine();
	if (ImGui::Button("Copy", ImVec2(100, 30)))
	{
		Copy_Object_Function();
	}
	SameLine();
	if (ImGui::Button("Cancel", ImVec2(100, 30)))
	{
		m_bCopyingObject = false;
		strcpy(m_CopyingObjectName, "");
		m_bPastePicking = false;
	}

	m_bPastePicking = (m_bCopyingObject == false) ? false : m_bPastePicking;

	SameLine();

	ImGui::Text(m_CopyingObjectName);

	if (RadioButton("Paste_Picking", m_bPastePicking))
	{
		m_bPastePicking = (m_bPastePicking == false) ? true : false;
	}

	if (m_bPastePicking == true)
	{
		Paste_Object_Function();
	}

	NewLine();
	Spacing();
	
	
	if (CollapsingHeader("Add Components"))
	{
		NewLine();

		Show_ComponentList();
		NewLine();

		if (ImGui::Button("Add Component", ImVec2(180, 50)))
		{
			Add_Component_Function();
		}
	}
	
	
	
}

void CInspectorWindow::Mode_Transform()
{
	CComponent* component = IMGUI->GetCurSelectComponent();
	CTransform* transform = dynamic_cast<CTransform*>(component);
	if (transform == nullptr) return;

	_vector originTranslation = transform->Get_State(CTransform::STATE_POSITION);
	_float3 originScale = transform->Get_Scaled();
	_float3 originRotation = transform->Get_Rotation_XYZ();

	memcpy(translation, &originTranslation, sizeof _float3);
	memcpy(scale, &originScale, sizeof _float3);
	memcpy(rotation, &originRotation, sizeof _float3);
	

	ImGui::Spacing();
	ImGui::DragFloat3("Translation", translation, 0.5f, -99999.f, 99999.f);
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::DragFloat3("Rotation", rotation, 0.5f, -99999.f, 99999.f))
	{
		if (originRotation.x != rotation[0])
			transform->Rotation(CTransform::AXIS_X, rotation[0]);
		if (originRotation.y != rotation[1])
			transform->Rotation(CTransform::AXIS_Y, rotation[1]);
		if (originRotation.z != rotation[2])
			transform->Rotation(CTransform::AXIS_Z, rotation[2]);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::DragFloat3("Scale", scale, 0.012f, 0.002f, 99999.f);
	if (m_bScaleLock == true)
	{
		if (originScale.x != scale[0])
		{
			scale[1] = scale[0];
			scale[2] = scale[0];
		}
		else if (originScale.y != scale[1])
		{
			scale[0] = scale[1];
			scale[2] = scale[1];
		}
		else if (originScale.z != scale[2])
		{
			scale[0] = scale[2];
			scale[1] = scale[2];
		}
	}
	SameLine();
	if (ImGui::RadioButton("lock", m_bScaleLock))
	{
		m_bScaleLock = (m_bScaleLock == false) ? true : false;
	}
	ImGui::Spacing();

	transform->Set_Position({ translation[0],translation[1],translation[2],1.f });
	transform->Scaled(_float3(scale));

	NewLine();

	
	if (ImGui::RadioButton("Place Object", m_bPlaceObject))
	{
		m_bPlaceObject = (m_bPlaceObject == false) ? true:false;
	}

	if (m_bPlaceObject == true)
		Place_Object(transform);
	
}

void CInspectorWindow::Mode_Collider()
{
	
}

void CInspectorWindow::Mode_Texture()
{
	
}

void CInspectorWindow::Mode_Shader()
{
	CComponent* component = IMGUI->GetCurSelectComponent();
	CGameObject* object = dynamic_cast<CGameObject*>(component->GetOwner());

	if (object)
	{
		object->Change_PassNum(1);
	}
}

void CInspectorWindow::Mode_Animation()
{
	
}

void CInspectorWindow::Mode_Model()
{
	static int i = 0;
	if (Button("Anim Index"))
	{
		CComponent* component = IMGUI->GetCurSelectComponent();
		CModel* model = dynamic_cast<CModel*>(component);
		model->Set_AnimIndex(i);
		i++;


	}
	ImGui::Text(to_string(i).c_str());
}

void CInspectorWindow::Mode_Renderer()
{
	

	// ImGui 윈도우 업데이트 루프 내부에서 다음 코드를 사용합니다.
	//ImGui::ListBox("Render Group", &m_CurRenderGroupSelectedItem, m_RenderGroupStrVec.data(), m_RenderGroupStrVec.size());

	//// 선택된 항목에 대한 작업을 수행합니다.
	//switch (selectedItem)
	//{
	//case 0:
	//	// 첫 번째 항목 선택됨
	//	break;
	//case 1:
	//	// 두 번째 항목 선택됨
	//	break;
	//case 2:
	//	// 세 번째 항목 선택됨
	//	break;
	//case 3:
	//	// 네 번째 항목 선택됨
	//	break;
	//default:
	//	break;
	//}
}

void CInspectorWindow::Mode_Navigation()
{
}

void CInspectorWindow::Mode_Default()
{
	ImGui::SetNextItemWidth(120);
	ImGui::InputText("Object Name", m_ObjectAddNameBuffer, IM_ARRAYSIZE(m_ObjectAddNameBuffer));

	if (ImGui::Button("Add GameObject", ImVec2(180, 50)))
	{
		Add_GameObject_Function();
	}
	SameLine();
}

void CInspectorWindow::Show_ComponentList()
{
	std::vector<int> searchResults;

	ImGui::InputText("Search", m_ComponentSearchBuffer, IM_ARRAYSIZE(m_ComponentSearchBuffer));
	ImGui::BeginChild("ScrollRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	for (int i = 0; i < m_PrototypesStrVec.size(); i++)
	{
		if (strstr(m_PrototypesStrVec[i].c_str(), m_ComponentSearchBuffer) != nullptr) // 검색어와 일치하는 경우에만 항목을 표시
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

void CInspectorWindow::Copy_Object_Function()
{
	// Copy 해서 복사본을 갖고 있게 해야됨.
	m_bCopyingObject = true;

	CDummyObject* object = dynamic_cast<CDummyObject*>(IMGUI->GetCurSelectComponent());
	wstring name = object->GetName();
	string objectName = CConversion::WstringToString(name);
	strcpy(m_CopyingObjectName, objectName.c_str());
	
	
}

void CInspectorWindow::Paste_Object_Function()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Mouse_Up(CInput_Device::DIMK_LBUTTON))
		m_bCopyLock = false;

	if (pGameInstance->Mouse_Pressing(CInput_Device::DIMK_LBUTTON) && m_bCopyLock == false)
	{
		// GetCurSelectComponent 가 아니라 Copy 된 Object를 처리 해야됨.
		CDummyObject* object = dynamic_cast<CDummyObject*>(IMGUI->GetCurSelectComponent());

		wstring name = object->GetName();
		CDummyObject* newObject = dynamic_cast<CDummyObject*>(pGameInstance->Copy_GameObject(TEXT("Layer_Object"), name));

		if (newObject == nullptr) return;

		if (Place_Object(newObject->m_pTransformCom) == E_FAIL)
		{
			pGameInstance->Delete_GameObject(TEXT("Layer_Object"), name);
		}

		m_bCopyLock = true;

		IMGUI->ChangeTree();
	}
	
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

	CTransform::TRANSFORMDESC desc = CTransform::TRANSFORMDESC(7.0, XMConvertToRadians(90.0f));

	for (auto& str : splitVector)
	{
		if (str.compare(RENDERER_A) == 0)
		{
			if (gameObject->m_pRendererCom != nullptr) 
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pRendererCom;
		}
		if (str.compare(TRANSFORM_A) == 0)
		{
			if (gameObject->m_pTransformCom != nullptr) 
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pTransformCom;
			pArg = (void*)&desc;
		}
		if (str.compare(VIBUFFER_A) == 0)
		{
			if (gameObject->m_pVIBufferCom != nullptr) 
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pVIBufferCom;

			gameObject->m_VIBufferPrototypeName = m_PrototypesVec[selectedComponentIndex].first;
		}
		if (str.compare(SHADER_A) == 0)
		{
			if (gameObject->m_pShaderCom != nullptr)
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pShaderCom;

			gameObject->m_ShaderPrototypeName = m_PrototypesVec[selectedComponentIndex].first;
		}
		if (str.compare(TEXTURE_A) == 0)
		{
			if (gameObject->m_pTextureCom != nullptr)
				return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pTextureCom;

			gameObject->m_TexturePrototypeName = m_PrototypesVec[selectedComponentIndex].first;
		}
		if (str.compare(MODEL_A) == 0)
		{
			if (gameObject->m_pModelCom != nullptr) return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pModelCom;

			gameObject->m_ModelPrototypeName = m_PrototypesVec[selectedComponentIndex].first;
		}
		if (str.compare(NAVIGATION_A) == 0)
		{
			if (gameObject->m_pNavigationCom != nullptr) return;

			name = str;
			FieldComp = (CComponent**)&gameObject->m_pNavigationCom;

			gameObject->m_NavigationPrototypeName = m_PrototypesVec[selectedComponentIndex].first;
		}
		
	}

	// Tchar* 변환 다시 처리하기

	wstring componentName = CConversion::StringToWstring(name);
	gameObject->Add_Component(LEVEL_TOOL, m_PrototypesVec[selectedComponentIndex].first.c_str(),
		componentName.c_str(), (CComponent**)FieldComp, pArg);
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
	if (str.compare("Model") == 0)
	{
		resultCom = (CComponent**)&gameObject->m_pModelCom;
	}
	if (str.compare(NAVIGATION_A) == 0)
	{
		resultCom = (CComponent**)&gameObject->m_pNavigationCom;
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
	wstring name = CConversion::StringToWstring(m_ObjectAddNameBuffer);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyObject"), TEXT("Layer_Object"), name);

	IMGUI->ChangeTree();
}

void CInspectorWindow::Delete_GameObject_Function()
{
	wstring objectName = IMGUI->GetCurSelectComponent()->GetName();

	CGameInstance::GetInstance()->Delete_GameObject(L"Layer_Object", objectName);

	IMGUI->PickingReset();

	IMGUI->ChangeTree();
}

HRESULT CInspectorWindow::Place_Object(CTransform* transformComp)
{
	if (CInput_Device::GetInstance()->Get_DIMouseState(CInput_Device::DIMK_LBUTTON) & 0x80)
	{
		CTransform* transform = transformComp;
		if (transform == nullptr) return E_FAIL;

		CDummyObject* terrain = dynamic_cast<CDummyObject*>(IMGUI->Find_GameObject(L"Terrain"));

		if (terrain == nullptr) return E_FAIL;

		CVIBuffer_Terrain* terrainBuffer = dynamic_cast<CVIBuffer_Terrain*>(terrain->m_pVIBufferCom);
		CTransform* terrainTransform = dynamic_cast<CTransform*>(terrain->m_pTransformCom);

		_float4 pickingPos;
		_int pickingResult = CCalculator::Picking_OnTerrain(g_hWnd, g_iWinSizeX, g_iWinSizeY, terrainBuffer, terrainTransform, &pickingPos);

		if (pickingResult == -1) return E_FAIL;

		transform->Set_Position(XMLoadFloat4(&pickingPos));

		m_bPlaceObject = false;
	}

	
	return S_OK;
}

_bool CInspectorWindow::Is_Use_Picking()
{
	return (m_bPlaceObject || m_bPastePicking);
}


