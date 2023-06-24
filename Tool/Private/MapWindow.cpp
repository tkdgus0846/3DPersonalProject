#include "stdafx.h"
#include "..\Public\MapWindow.h"
#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "GameObject.h"
#include <DummyObject.h>
#include "Calculator.h"

HRESULT CMapWindow::Initialize(const WINDOWDESC& desc)
{
	HRESULT result = __super::Initialize(desc);

	only_load_button = true;

	m_eBrushMode = BM_SCULPT;
	m_fBrushSize = (m_fMaxBrushSize + m_fMinBrushSize) * 0.5f;

	m_TerrainSize[0] = 100;
	m_TerrainSize[1] = 100;

	m_TerrainInterval = 1.f;

	m_PrototypesTextures = CGameInstance::GetInstance()->Get_Prototypes_ByVector();
	m_PrototypesNavMeshes = m_PrototypesTextures;

	for (auto& item : m_PrototypesTextures)
	{
		string name = CConversion::WstringToString(item.first);
		m_PrototypesTexturesStr.push_back(name);
		m_PrototypesNavMeshesStr.push_back(name);
	}

	auto iter1 = m_PrototypesTextures.begin();
	auto iter2 = m_PrototypesTexturesStr.begin();
	while (true)
	{
		if ((*iter2).find("Texture") == std::string::npos)
		{
			iter1 = m_PrototypesTextures.erase(iter1);
			iter2 = m_PrototypesTexturesStr.erase(iter2);
		}
		else
		{
			iter1++;
			iter2++;
		}
		if (iter1 == m_PrototypesTextures.end()) break;
	}

	iter1 = m_PrototypesNavMeshes.begin();
	iter2 = m_PrototypesNavMeshesStr.begin();
	while (true)
	{
		if ((*iter2).find("Navigation") == std::string::npos)
		{
			iter1 = m_PrototypesNavMeshes.erase(iter1);
			iter2 = m_PrototypesNavMeshesStr.erase(iter2);
		}
		else
		{
			iter1++;
			iter2++;
		}
		if (iter1 == m_PrototypesNavMeshes.end()) break;
	}

	return result;
}

CMapWindow* CMapWindow::Create(const WINDOWDESC& windowDesc)
{
	CMapWindow* newWindow = new CMapWindow();
	if (FAILED(newWindow->Initialize(windowDesc)))
	{
		Safe_Release(newWindow);
		MSG_BOX("MapWindow Create Failed!");
		return nullptr;
	}
	return newWindow;
}

void CMapWindow::Tick(_double timeDelta)
{
	if (m_Desc.bOpen == true)
		Brush_Function();
}

void CMapWindow::Set_Render_NavMesh(_bool bRender)
{
	if (m_pCurNavigation == nullptr) return;

	m_pCurNavigation->Set_Render(bRender);
}

void CMapWindow::Set_CurTerrain(CDummyObject* pTerrain)
{
	if (pTerrain == nullptr) return;
	m_pCurTerrain = pTerrain;
	m_pCurTerrainBuffer = (CVIBuffer_Terrain*)pTerrain->m_pVIBufferCom;
	m_pCurNavigation = (CNavigation*)pTerrain->m_pNavigationCom;
}

void CMapWindow::Rendering()
{
	Spacing();
	BrushTool();
	Spacing();
	TerrainTool();
	Spacing();
	NavMeshTool();
}

void CMapWindow::BrushTool()
{
	if (ImGui::CollapsingHeader("BrushTool"))
	{
		Spacing();
		if (RadioButton("Sculpt", m_eBrushMode == BM_SCULPT))
		{
			m_eBrushMode = BM_SCULPT;
		}
		SameLine();
		if (RadioButton("Erosion", m_eBrushMode == BM_EROSION))
		{
			m_eBrushMode = BM_EROSION;
		}
		SameLine();
		if (RadioButton("Flatten", m_eBrushMode == BM_FLATTEN))
		{
			m_eBrushMode = BM_FLATTEN;
		}

		float scroll = ImGui::GetIO().MouseWheel;
		if (scroll != 0.0f)
		{
			m_fBrushSize += scroll; // 휠의 움직임에 따라 변수 값을 조정

			if (m_pCurTerrain != nullptr)
				m_pCurTerrain->m_BrushSize = m_fBrushSize / 2.f;

			if (m_fBrushSize > m_fMaxBrushSize) 
				m_fBrushSize = m_fMaxBrushSize;
			if (m_fBrushSize < m_fMinBrushSize)
				m_fBrushSize = m_fMinBrushSize;
		}

		/*Spacing(); Spacing(); Spacing(); Spacing();*/
		NewLine();
		SeparatorText("Brush Default Settings");
		SliderFloat("BrushSize", &m_fBrushSize, m_fMinBrushSize, m_fMaxBrushSize, "%.2f");
		SliderFloat("BrushPower", &m_fBrushPower, m_fMinBrushPower, m_fMaxBrushPower, "%.2f");

		NewLine();

		/*switch (m_eBrushMode)
		{
		case CMapWindow::BM_SCULPT:
			SculptBrush();
			break;
		case CMapWindow::BM_EROSION:
			ErosionBrush();
			break;
		case CMapWindow::BM_FLATTEN:
			FlattenBrush();
			break;
		case CMapWindow::BM_END:
			break;
		default:
			break;
		}*/
	}
}

void CMapWindow::ErosionBrush()
{
	NewLine();
	SeparatorText("Erosion");
}

void CMapWindow::SculptBrush()
{
	NewLine();
	SeparatorText("Sculpt");
}

void CMapWindow::FlattenBrush()
{
	NewLine();
	SeparatorText("Flatten");
}

void CMapWindow::Brush_Function()
{
	if (m_pCurTerrain == nullptr || m_pCurTerrainBuffer == nullptr || m_bBrushOn == false) return;

	if (m_pCurTerrain->m_pBrushTexture == nullptr) return;

	m_pCurTerrain->m_pBrushTexture->Bind_ShaderResources(m_pCurTerrain->m_pShaderCom, "g_BrushTexture");
	m_pCurTerrain->m_pShaderCom->Bind_RawValue("g_fBrushRadius", &m_pCurTerrain->m_BrushSize, sizeof(_float));

	CVIBuffer_Terrain* terrainBuffer = (CVIBuffer_Terrain*)m_pCurTerrain->m_pVIBufferCom;

	_int rayHit = -1;
	if ((rayHit = CCalculator::Picking_OnTerrain(g_hWnd, g_iWinSizeX, g_iWinSizeY, terrainBuffer, m_pCurTerrain->m_pTransformCom, &m_pCurTerrain->m_BrushPos)) >= 0)
	{
		m_pCurTerrain->m_pShaderCom->Bind_Float4("g_vBrushPos", &m_pCurTerrain->m_BrushPos);
		if (CGameInstance::GetInstance()->Mouse_Pressing(CInput_Device::DIMK_LBUTTON))
		{
			_float power = m_fBrushPower;

			if (m_eBrushMode == BM_EROSION) power *= -1.f;

			terrainBuffer->Picking_Height_Cos(rayHit, m_pCurTerrain->m_BrushSize, power, XMLoadFloat4(&m_pCurTerrain->m_BrushPos));
		}
		
	}
}

void CMapWindow::TerrainTool()
{
	if (ImGui::CollapsingHeader("TerrainTool"))
	{
		Spacing();
		/*Text("X: ");
		SameLine();*/
		SeparatorText("TerrainSize");
		InputInt(" => X", m_TerrainSize);
		InputInt(" => Z", m_TerrainSize+1);
		InputFloat(" => Interval", &m_TerrainInterval);
		
		NewLine();
		SeparatorText("Texture");
		// 텍스쳐 선택해서 집어넣기. (프로토타입을 고르게 해도 무방할것같다.)
		// 높이맵 (아마도 직접 그린 높이맵을 불러올거같음)
		ShowTextureComponentList();
		
		

		SetCursorPosX(200.f);
		if (Button("Make Terrain"))
		{
			// 그냥 terrainName 을 하지말고 
			wstring terrainName = L"Terrain";
			CGameInstance* pGameInstance = CGameInstance::GetInstance();

			
			if (m_bLoad == false && m_pCurTerrain != nullptr)
			{
				DeleteCurTerrain();
			}
			
			CDummyObject* terrain;
			if (m_bLoad == false)
			{
				terrain = (CDummyObject*)pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DummyObject"), terrainName);
				m_pCurTerrain = terrain;

				m_pCurTerrainBuffer = CVIBuffer_Terrain::Create(IMGUI->GetDevice(), IMGUI->GetContext(), m_TerrainSize[0], m_TerrainSize[1]);
			}
			else
			{
				terrain = m_pCurTerrain;
			}
			
			terrain->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"), SHADER_W, (CComponent**)&terrain->m_pShaderCom);
			terrain->m_ShaderPrototypeName = TEXT("Prototype_Component_Shader_VtxNorTex");

			terrain->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Brush"), TEXT("Texture_Brush"), (CComponent**)&terrain->m_pBrushTexture);

			if (selectedComponentIndex == -1)
			{
				Safe_Release(m_pCurTerrain);
				Safe_Release(m_pCurTerrainBuffer);
				m_pCurTerrain = nullptr;
				m_pCurTerrainBuffer = nullptr;
				return;
			}
			wstring prototypeName = CConversion::StringToWstring(m_PrototypesTexturesStr[selectedComponentIndex]);
			terrain->Add_Component(LEVEL_TOOL, prototypeName.c_str(), TEXTURE_W, (CComponent**)&terrain->m_pTextureCom);
			terrain->m_TexturePrototypeName = prototypeName.c_str();

			terrain->m_pVIBufferCom = m_pCurTerrainBuffer;

			terrain->Add_Component(m_pCurTerrainBuffer, L"VIBuffer");
			
			terrain->Change_PassNum(1);

			pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Layer_Object"), terrainName, terrain);

			m_pCurTerrain->Change_PassNum(1);

			m_bLoad = false;
			
		}
		SameLine();

		if (m_pCurTerrain != nullptr && m_pCurTerrainBuffer != nullptr)
		{
			PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 버튼 색상 변경	

			
			if (Button("Bake Terrain"))
			{
				m_pCurTerrainBuffer->Backing_Normal_All();
			}

			ImGui::PopStyleColor();
		}
		
		NewLine();
	}

	
}

void CMapWindow::ShowTextureComponentList()
{
	std::vector<int> searchResults;

	ImGui::InputText("Search", m_ComponentSearchBuffer, IM_ARRAYSIZE(m_ComponentSearchBuffer));
	ImGui::BeginChild("ScrollRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	for (int i = 0; i < m_PrototypesTexturesStr.size(); i++)
	{
		if (strstr(m_PrototypesTexturesStr[i].c_str(), m_ComponentSearchBuffer) != nullptr) // 검색어와 일치하는 경우에만 항목을 표시
		{
			searchResults.push_back(i);
		}
	}

	for (int i = 0; i < searchResults.size(); i++)
	{
		int index = searchResults[i];
		if (ImGui::Selectable(m_PrototypesTexturesStr[index].c_str()))
		{
			selectedComponentIndex = index;
		}
	}

	ImGui::EndChild();

	if (selectedComponentIndex >= 0 && selectedComponentIndex < m_PrototypesTexturesStr.size())
	{
		NewLine();
		BulletText(m_PrototypesTexturesStr[selectedComponentIndex].c_str());
		NewLine();
	}
}

void CMapWindow::Open_Function()
{
	if (m_pCurTerrain != nullptr)
	{
		m_pCurTerrain->Change_PassNum(1);
	}
}

void CMapWindow::Close_Function()
{
	if (m_pCurTerrain != nullptr && m_pCurTerrainBuffer != nullptr)
	{
		m_pCurTerrain->Change_PassNum(0);
		m_pCurTerrainBuffer->Backing_Normal_All();

		Save_Function();
		/*m_pCurTerrain = nullptr;
		m_pCurTerrainBuffer = nullptr;
		m_pCurNavigation = nullptr;*/
	}
}

void CMapWindow::Save_Function()
{
	IMGUI->SaveMode();
	IMGUI->SaveLoad_By(MAP_WINDOW_NAME);

	ImGuiFileDialog::Instance()->OpenDialog(IMGUI->SaveLoad_Dialog_Key().c_str(), "Choose File", ".dat", "../../Terrains/");
}

void CMapWindow::Load_Function()
{
	IMGUI->LoadMode();
	IMGUI->SaveLoad_By(MAP_WINDOW_NAME);

	ImGuiFileDialog::Instance()->OpenDialog(IMGUI->SaveLoad_Dialog_Key().c_str(), "Choose File", ".dat", "../../Terrains/");

	m_bLoad = true;
}

void CMapWindow::DeleteCurTerrain()
{
	CGameInstance::GetInstance()->Delete_GameObject(L"Layer_Object", m_pCurTerrain->GetName());
	m_pCurTerrain = nullptr;
	m_pCurTerrainBuffer = nullptr;
	m_pCurNavigation = nullptr;
}

void CMapWindow::NavMeshTool()
{
	if (ImGui::CollapsingHeader("NavMeshTool"))
	{
		Spacing();

		if (m_bNavMeshEditMode == false)
		{
			ShowNavigationList();

			SetCursorPosX(260.f);
			if (Button("Add"))
			{
				if (m_pCurTerrain != nullptr && selectedNavigationIndex != -1)
				{
					m_pCurTerrain->Add_Component(LEVEL_TOOL, CConversion::StringToWstring(m_PrototypesNavMeshesStr[selectedNavigationIndex]).c_str(), TEXT("Navigation"), (CComponent**)&m_pCurTerrain->m_pNavigationCom);

					m_pCurNavigation = m_pCurTerrain->m_pNavigationCom;
				}

			}
			SameLine();
			if (Button("Edit"))
			{
				if (m_pCurNavigation == nullptr || m_pCurTerrain == nullptr) return;
				m_pCurTerrain->Change_PassNum(0);
				m_bBrushOn = { false };
				m_bNavMeshEditMode = { true };
			}
		}
		else
		{
			EditNavigationTool();
			SetCursorPosX(200.f);
			PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 버튼 색상 변경	
			//PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

			if (Button("Cancel", {100,30}))
			{
				m_pCurNavigation->Reset_Selected_AllCells();
				m_PickingVec.clear();
				m_PickingIndexVec.clear();
			}

			SameLine();

			if (Button("Exit", { 100,30 }))
			{
				m_bBrushOn = { true };
				m_bNavMeshEditMode = false;
				m_pCurTerrain->Change_PassNum(1);

				m_pCurNavigation->Reset_Selected_AllCells();
				m_PickingVec.clear();
				m_PickingIndexVec.clear();
			}

			//::PopStyleColor(); // 이전의 스타일로 복원
			ImGui::PopStyleColor(); // 이전의 스타일로 복원
			
		}
	}
}

void CMapWindow::ShowNavigationList()
{
	std::vector<int> searchResults;

	ImGui::InputText("SearchNav", m_NavigationSearchBuffer, IM_ARRAYSIZE(m_NavigationSearchBuffer));
	ImGui::BeginChild("ScrollRegionNav", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	for (int i = 0; i < m_PrototypesNavMeshesStr.size(); i++)
	{
		if (strstr(m_PrototypesNavMeshesStr[i].c_str(), m_NavigationSearchBuffer) != nullptr) // 검색어와 일치하는 경우에만 항목을 표시
		{
			searchResults.push_back(i);
		}
	}

	for (int i = 0; i < searchResults.size(); i++)
	{
		int index = searchResults[i];
		if (ImGui::Selectable(m_PrototypesNavMeshesStr[index].c_str()))
		{
			selectedNavigationIndex = index;
		}
	}

	ImGui::EndChild();

	if (selectedNavigationIndex >= 0 && selectedNavigationIndex < m_PrototypesNavMeshesStr.size())
	{
		NewLine();
		BulletText(m_PrototypesNavMeshesStr[selectedNavigationIndex].c_str());
		NewLine();
	}
}

void CMapWindow::EditNavigationTool()
{
	if (m_pCurNavigation == nullptr || m_pCurTerrainBuffer == nullptr || m_pCurTerrain == nullptr)  
		return;

	if (m_PickingVec.size() == 1)
	{
		// Transform 창 띄우기.
		_float pos[3];
		pos[0] = m_PickingVec[0].x;
		pos[1] = m_PickingVec[0].y;
		pos[2] = m_PickingVec[0].z;

		DragFloat3("Translation", pos, 0.5f);

		if (m_PickingVec[0].y != pos[1])
		{
			int i = 1;
		}

		m_PickingVec[0].x = pos[0];
		m_PickingVec[0].y = pos[1];
		m_PickingVec[0].z = pos[2];

		m_pCurNavigation->Translate(m_PickingIndexVec[0], XMLoadFloat3(&m_PickingVec[0]));
	}

	if (CGameInstance::GetInstance()->Mouse_Down(CInput_Device::DIMK_LBUTTON))
	{
		_float4 resultPos;
		_int index;

		index = CCalculator::Picking_Sphere(g_hWnd, g_iWinSizeX, g_iWinSizeY, m_pCurNavigation->Get_All_Sphere(), &resultPos);

		if (index >= 0)
		{
			_float3 pickingPos = m_pCurNavigation->Picking_Cell(index);
			m_PickingVec.push_back(pickingPos);
			m_PickingIndexVec.push_back(index);

			if (m_PickingVec.size() == 3)
			{
				m_pCurNavigation->Reset_Selected_AllCells();
				m_pCurNavigation->Add_Cell(m_PickingVec.data());

				m_PickingVec.clear();
				m_PickingIndexVec.clear();
			}
		}
		else
		{
			if (m_PickingVec.size() < 2) return;

			index = CCalculator::Picking_OnTerrain(g_hWnd, g_iWinSizeX, g_iWinSizeY, m_pCurTerrainBuffer, m_pCurTerrain->m_pTransformCom, &resultPos);

			if (index >= 0)
			{
				m_pCurNavigation->Reset_Selected_AllCells();

				m_PickingVec.push_back(_float3(resultPos.x, resultPos.y + 0.1f, resultPos.z));
				m_pCurNavigation->Add_Cell(m_PickingVec.data());

				m_PickingVec.clear();
				m_PickingIndexVec.clear();
			}
		}
	}
	
	// 1. 광선을 쏴서 Cell 을 클릭할수 있게한다.
	// 2. Cell 추가 모드
	// 3. Cell 지우기 모드


	// 지울때는 어떻게 지울것인가? 선택된 점들을 빨갛게 표시하면 어떨까?
}

void CMapWindow::Save_Terrain(const wstring& name)
{
	if (m_pCurTerrain != nullptr && m_pCurTerrainBuffer != nullptr)
	{
		
		wstring prototypeName = TEXT("Prototype_Component_VIBuffer_Terrain_") + name;
		m_pCurTerrain->m_VIBufferPrototypeName = prototypeName;
		m_pCurTerrainBuffer->Save_TerrainBuffer(name);
		
		if (m_pCurNavigation != nullptr)
		{
			Save_Navigation(name);
		}
	}
}

void CMapWindow::Load_Terrain(const wstring& name)
{
	// 기존에 있던놈 지우는 작업도 해야됨.
	if (m_pCurTerrain != nullptr && m_pCurTerrainBuffer != nullptr)
	{
		DeleteCurTerrain();
		
	}
	
	// 경로 가공해서 Create 할까? 
	

	wstring prototypeName = TEXT("Prototype_Component_VIBuffer_Terrain_") + name;
	wstring terrain = L"Terrain";
	wstring viBufferName = L"VIBuffer";

	CDummyObject* terrainObject = (CDummyObject*)CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_DummyObject"), terrain);

	terrainObject->Add_Component(LEVEL_TOOL, prototypeName.c_str(), viBufferName, (CComponent**)&m_pCurTerrainBuffer);

	m_pCurTerrain = terrainObject;


	Load_Navigation(name);

	m_bLoad = true;
	

}

void CMapWindow::Save_Navigation(const wstring& name)
{
	if (m_pCurNavigation == nullptr || m_pCurTerrain == nullptr || m_pCurTerrainBuffer == nullptr) return;

	wstring prototypeName = TEXT("Prototype_Component_Navigation_") + name;

	m_pCurTerrain->m_NavigationPrototypeName = prototypeName;
	m_pCurNavigation->Save_Navigation(name);
}

void CMapWindow::Load_Navigation(const wstring& name)
{
	wstring prototypeName = TEXT("Prototype_Component_Navigation_") + name;
	wstring comName = L"Navigation";

	m_pCurTerrain->Add_Component(LEVEL_TOOL, prototypeName.c_str(), comName, (CComponent**)&m_pCurNavigation);
	m_pCurTerrain->m_pNavigationCom = m_pCurNavigation;
}

void CMapWindow::Save_Texture()
{
}
