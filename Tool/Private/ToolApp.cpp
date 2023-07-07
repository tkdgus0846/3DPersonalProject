#include "stdafx.h"
#include "ToolApp.h"

#include "GameInstance.h"
#include "Level_Tool.h"

#include <ImGuiManager.h>
#include "DummyObject.h"
#include "Camera_Main.h"


CToolApp::CToolApp()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CToolApp::Initialize()
{
	CoInitializeEx(nullptr, 0);
	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WM_WIN;

	/* 엔진을 초기화한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_TOOL)))
		return E_FAIL;
	
	IMGUI->Initialize(m_pDevice, m_pContext);
	

	return S_OK;
}

void CToolApp::Tick(_float TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	
	IMGUI->Tick(TimeDelta);
	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CToolApp::Render()
{
	if (nullptr == m_pGameInstance ||
		nullptr == m_pRenderer)
		return E_FAIL;

	
	
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.7f, 0.7f, 0.7f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();


	
	m_pRenderer->Draw_RenderGroup();

	
	IMGUI->Render();

	m_pGameInstance->Present();


	m_pGameInstance->Reset_Camera();
	
	

	return S_OK;
}

HRESULT CToolApp::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

#pragma region Component 추가
	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Shader_Vtxtex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Vtxtex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Cube"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_3.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png")))))
		return E_FAIL; 

	

	/* For.Prototype_Component_Texture_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Cobbolstone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SilverhillsBuilding_Cobblestone02_bc.dds")))))
		return E_FAIL;


	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp"),  CVIBuffer_Terrain::TYPE::DYNAMIC))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Terrain_NoHeightMap*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain_100x100"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100, 1.0f))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain_200x200"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 200, 200, 1.0f))))
	//	return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Terrain_NoHeightMap*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain_500x500"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 500, 500, 1.0f))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain_1000x1000"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 1000, 1000, 1.0f))))
	//	return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTexCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxTexCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTexCube.hlsl"), VTXPOSTEXCUBE_DECL::Elements, VTXPOSTEXCUBE_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
		return E_FAIL;

	////////////////////// 터레인 /////////////////////////
	path terrainPath = L"../../Terrains/";
	wstring prototypeName = TEXT("Prototype_Component_VIBuffer_Terrain_");
	for (const directory_entry& entry : directory_iterator(terrainPath))
	{
		if (entry.path().extension() == L".dat")
		{
			wstring FullName = prototypeName + entry.path().filename().stem().wstring();
			wstring FullPath = terrainPath.wstring() + entry.path().filename().stem().wstring() + L".dat";

			m_pGameInstance->Add_Prototype(LEVEL_TOOL, FullName.c_str(),
				CVIBuffer_Terrain::Create(m_pDevice, m_pContext, FullPath, CVIBuffer_Terrain::TYPE::DYNAMIC));
		}
	}

	///////////////////////// 네뷔 메쉬들 ////////////////////////
	path navPath = L"../../NavMeshes/";
	prototypeName = TEXT("Prototype_Component_Navigation_");
	for (const directory_entry& entry : directory_iterator(navPath))
	{
		if (entry.path().extension() == L".dat")
		{
			wstring FullName = prototypeName + entry.path().filename().stem().wstring();
			wstring FullPath = navPath.wstring() + entry.path().filename().stem().wstring() + L".dat";

			m_pGameInstance->Add_Prototype(LEVEL_TOOL, FullName.c_str(),
				CNavigation::Create(m_pDevice, m_pContext, FullPath.c_str()));
		}
	}


	/////////////////////////////////////
	///////////////////////////////////////
	/////////////// 모델 //////////////////
	///////////////////////////////////
	////////////////////////////////

	_matrix		PivotMatrix = XMMatrixIdentity();
	
	///////////// 논애님 모델들 ///////////////
	path modelPath = "../../ExtractModels/NonAnimModels/";
	prototypeName = TEXT("Prototype_Component_NonAnim_Model_");
	for (const directory_entry& entry : recursive_directory_iterator(modelPath))
	{
		if (entry.path().extension() == L".dat")
		{
			wstring FullName = prototypeName + entry.path().filename().stem().wstring();
			string FullPath = modelPath.string()+ entry.path().filename().stem().string()+ "/" + entry.path().filename().stem().string() + ".dat";

			m_pGameInstance->Add_Prototype(LEVEL_TOOL, FullName,
				CModel::Create(m_pDevice, m_pContext, FullPath.c_str(), PivotMatrix));
		}
	}

	//////////// 애님 모델들 //////////////////////////

	/* For.Prototype_Component_Model_Fiona */
	modelPath = "../../ExtractModels/AnimModels/";
	prototypeName = TEXT("Prototype_Component_Anim_Model_");
	for (const directory_entry& entry : recursive_directory_iterator(modelPath))
	{
		if (entry.path().extension() == L".dat")
		{
			wstring FullName = prototypeName + entry.path().filename().stem().wstring();
			string FullPath = modelPath.string() + entry.path().filename().stem().string() + "/" + entry.path().filename().stem().string() + ".dat";

			m_pGameInstance->Add_Prototype(LEVEL_TOOL, FullName,
				CModel::Create(m_pDevice, m_pContext, FullPath.c_str(), PivotMatrix));
		}
	}
	

	/*PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_NPCPaladin_Standard_LOD00_rig"),
		CModel::Create(m_pDevice, m_pContext, "../../ExtractModels/AnimModels/NPCPaladin_Standard_LOD00_rig/NPCPaladin_Standard_LOD00_rig.dat", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_crow_final"),
		CModel::Create(m_pDevice, m_pContext, "../../ExtractModels/AnimModels/crow_final/crow_final.dat", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, "../../ExtractModels/AnimModels/Fiona/Fiona.dat", PivotMatrix))))
		return E_FAIL;*/


	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_ForestSpirit"),
		CModel::Create(m_pDevice, m_pContext, "../../ExtractModels/AnimModels/ForestSpirit/ForestSpirit.dat", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_3924"),
		CModel::Create(m_pDevice, m_pContext, "../../ExtractModels/AnimModels/3924/3924.dat", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_NPCNecromancer_SkeletonPriest"),
		CModel::Create(m_pDevice, m_pContext, "../../ExtractModels/AnimModels/NPCNecromancer_SkeletonPriest/NPCNecromancer_SkeletonPriest.dat", PivotMatrix))))
		return E_FAIL;*/


#pragma endregion  Component 추가
	
#pragma region 게임오브젝트 추가
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DummyObject"),
		CDummyObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Main"),
		CCamera_Main::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion 게임오브젝트 추가

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CToolApp::Open_Level(LEVELID eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_TOOL, CLevel_Tool::Create(m_pDevice, m_pContext));
	return S_OK;
}

CToolApp* CToolApp::Create()
{
	CToolApp* pInstance = new CToolApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CToolApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolApp::Free()
{
	IMGUI->DestroyInstance();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	/* 게임인스턴스 객체의 레퍼런스 카운트를 감소시켰다. */
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
