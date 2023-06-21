#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Component_Manager.h"
#include "DataParsing.h"

#include "Layer.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::GetInstance() }
	, m_pLevel_Manager { CLevel_Manager::GetInstance() }
	, m_pObject_Manager { CObject_Manager::GetInstance() }
	, m_pTimer_Manager { CTimer_Manager::GetInstance() }
	, m_pComponent_Manager { CComponent_Manager::GetInstance() }
	, m_pPipeLine { CPipeLine::GetInstance() }
	, m_pInput_Device{ CInput_Device::GetInstance() }
	, m_pLight_Manager{ CLight_Manager::GetInstance() }
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pInput_Device);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* 디바이스 초기화. (그래픽, 사운드, 입력장치) */
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDevice, ppContext)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Ready_DInput(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 오브젝트 매니져, 컴포넌트 매니져들은 Reserve 한다.  */
	if (FAILED(m_pObject_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;	

	if (FAILED(m_pComponent_Manager->Reserve_Containers(iNumLevels)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pPipeLine ||
		nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Update_DInput();

	m_pObject_Manager->Tick(TimeDelta);

	m_pPipeLine->Tick();
		
	m_pObject_Manager->Late_Tick(TimeDelta);

	m_pLevel_Manager->Tick(TimeDelta);

	m_pLevel_Manager->Late_Tick(TimeDelta);

}

void CGameInstance::Clear_LevelResources(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return;

	m_pObject_Manager->Clear_LevelResources(iLevelIndex);
	m_pComponent_Manager->Clear_LevelResources(iLevelIndex);

}

void CGameInstance::ResizeBuffers(_uint& g_ResizeWidth, _uint& g_ResizeHeight)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->ImguiForResizeBuffer(g_ResizeWidth, g_ResizeHeight);
}


HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_byte CGameInstance::Get_DIKeyState(_ubyte ubyKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(ubyKeyID);
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouseID);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{

	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);
}

bool CGameInstance::Key_Pressing(_ubyte ubyKey)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Key_Pressing(ubyKey);
}

bool CGameInstance::Key_Down(_ubyte ubyKey)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Key_Down(ubyKey);
}

bool CGameInstance::Key_Up(_ubyte ubyKey)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Key_Up(ubyKey);
}

bool CGameInstance::Mouse_Down(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Mouse_Down(eMouseID);
}

bool CGameInstance::Mouse_Pressing(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Mouse_Pressing(eMouseID);
}

bool CGameInstance::Mouse_Up(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Mouse_Up(eMouseID);
}

_double CGameInstance::Get_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return  0.0;

	return m_pTimer_Manager->Get_Timer(pTimerTag);	
}

void CGameInstance::Set_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Set_Timer(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pObject_Manager->Set_CurLevelIndex(iLevelIndex);

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);	
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

CGameObject* CGameInstance::Add_GameObject(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pLayerTag, wstring& pObjName, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, pPrototypeTag, pLayerTag, pObjName, pArg);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const wstring& pLayerTag, wstring& pObjName, CGameObject* object)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex,  pLayerTag, pObjName, object);
}

CGameObject* CGameInstance::Copy_GameObject(const wstring& layerTag, wstring& objectName)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	if (nullptr == m_pLevel_Manager)
		return nullptr;

	return m_pObject_Manager->Copy_Object(m_pLevel_Manager->Get_CurLevelIndex(), layerTag, objectName);
}

HRESULT CGameInstance::Delete_GameObject(const wstring& pLayerTag, const wstring& pObjName)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_GameObject(m_pLevel_Manager->Get_CurLevelIndex(), pLayerTag, pObjName);
}

CGameObject* CGameInstance::Find_GameObject(const wstring& pLayerTag, const wstring& pObjName)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	if (nullptr == m_pLevel_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(m_pLevel_Manager->Get_CurLevelIndex(), pLayerTag, pObjName);
}

HRESULT CGameInstance::Save_CurLevel(const _tchar* curLevelName)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	
	LevelParsingData* levelData = new LevelParsingData(curLevelName);

	m_pObject_Manager->Save_Data(0, levelData);
	
	Safe_Delete(levelData);
	return S_OK;
}

HRESULT CGameInstance::Load_CurLevel(const _tchar* curLevelName, _bool bIsTool)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;


	LevelParsingData* levelData = new LevelParsingData(curLevelName);

	if (bIsTool == false)
		m_pObject_Manager->Load_Data(0, levelData);
	else
		m_pObject_Manager->Load_Tool_Objects(levelData);
	Safe_Delete(levelData);
	return S_OK;
}

list<CGameObject*> CGameInstance::Get_All_Objects()
{
	list<CGameObject*> resultObjList;
	for (auto& layer : *m_pObject_Manager->Get_LayersMapPtr())
	{
		for (auto& obj : *layer.second->Get_GameObjectsList())
			resultObjList.push_back(obj);
	}
	return resultObjList;
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	
}

vector<pair<const _tchar*, class CComponent*>> CGameInstance::Get_Prototypes_ByVector()
{
	if (nullptr == m_pComponent_Manager)
		return vector<pair<const _tchar*, class CComponent*>>();
	return m_pComponent_Manager->Get_Prototypes_ByVector();
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eTransformState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
}

_float4 CGameInstance::Get_CamPosition() const
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

const CLight::LIGHTDESC* CGameInstance::Get_Light(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_Light(iIndex);
}

HRESULT CGameInstance::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Lights(pDevice, pContext, LightDesc);
}

void CGameInstance::Release_Engine()
{	
	CGameInstance::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();	

	CTimer_Manager::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}
