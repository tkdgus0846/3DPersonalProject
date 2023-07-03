#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "Engine_Enum.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT Client::CMainApp::Initialize()
{
	GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WM_WIN;

	/* 엔진을 초기화한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst,LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Gara()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(TimeDelta);

	m_pGameInstance->Check_Collision(COL_PLAYER, COL_ENEMY);
	m_pGameInstance->Check_Collision(COL_PLAYER, COL_DETECTION);
	m_pGameInstance->Check_Collision(COL_PLAYERWEAPON, COL_ENEMY);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance || 
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.0f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();
#ifdef _DEBUG
	m_pGameInstance->Render_Collider();
#endif


	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::PostRender()
{
	m_pGameInstance->Reset_ColGroup();
	m_pGameInstance->Reset_Camera();

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	wstring prototypeName = L"Prototype_Component_Renderer";

	/* Prototype_Component_Renderer */
	//prototypeName = L"Prototype_Component_Renderer";
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, prototypeName,
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Transform */
	prototypeName = L"Prototype_Component_Transform";
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, prototypeName, CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Shader_Vtxtex */
	prototypeName = L"Prototype_Component_Shader_Vtxtex";
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, prototypeName, CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtxtex.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, wstring(L"Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, wstring(L"Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	///* Prototype_Component_Shader_VtxNortex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, wstring(L"Prototype_Component_Shader_VtxNortex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNortex.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTexCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, wstring(L"Prototype_Component_Shader_VtxTexCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTexCube.hlsl"), VTXPOSTEXCUBE_DECL::Elements, VTXPOSTEXCUBE_DECL::iNumElements))))
		return E_FAIL;



	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pTexture = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 128;
	TextureDesc.Height = 128;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA	SubResourceData;
	ZeroMemory(&SubResourceData, sizeof SubResourceData);

	_ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];
	ZeroMemory(pPixel, sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height);

	for (size_t i = 0; i < TextureDesc.Height; i++)
	{
		for (size_t j = 0; j < TextureDesc.Width; j++)
		{
			size_t iIndex = i * TextureDesc.Width + j;

			if (j < 64)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	SubResourceData.pSysMem = pPixel;
	SubResourceData.SysMemPitch = TextureDesc.Width * 4;
	// 텍스쳐 가로 줄의 크기

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture)))
		return E_FAIL;


	pPixel[0] = D3DCOLOR_ARGB(255, 0, 0, 255);

	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;

	m_pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD/*D3D11_MAP_WRITE_NO_OVERWRITE */, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, pPixel, sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height);

	m_pContext->Unmap(pTexture, 0);


	if (FAILED(SaveDDSTextureToFile(m_pContext, pTexture, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds"))))
		return E_FAIL;

	Safe_Release(pTexture);
	Safe_Delete_Array(pPixel);


	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;	

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));	
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	/* 게임인스턴스 객체의 레퍼런스 카운트를 감소시켰다. */
	Safe_Release(m_pGameInstance);	

	CGameInstance::Release_Engine();	
}
